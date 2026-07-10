`timescale 1ns / 1ps

module audio_clip_i2s_player (
    input  wire audio_clk,   // 12.288 MHz audio master clock
    input  wire reset,       // active-high reset

    input  wire SDATA_I,     // codec ADC data, unused in this playback-only project

    output reg  BCLK_O,      // I2S bit clock: 3.072 MHz
    output reg  LRCLK_O,     // I2S left/right clock: 48 kHz
    output reg  SDATA_O      // I2S DAC data to codec
);

    /*
     * Stored audio clip format:
     *
     * Source sample rate: 12 kHz
     * Sample width:       16-bit signed PCM
     * Channels:           mono
     * Duration:           5 seconds
     *
     * Valid audio samples:
     * 12,000 samples/sec x 5 sec = 60,000 samples
     *
     * Codec output format:
     * 48 kHz stereo I2S-style stream
     *
     * Therefore:
     * Each stored 12 kHz sample is repeated 4 times:
     * 48 kHz / 12 kHz = 4
     */

    localparam integer SAMPLE_COUNT  = 60000;   // actual valid audio samples
    localparam integer ADDR_WIDTH    = 16;      // 2^16 = 65536
    localparam integer ROM_DEPTH     = 65536;   // physical ROM depth
    localparam integer SAMPLE_REPEAT = 4;       // 12 kHz sample repeated to 48 kHz output

    /*
     * XPM ROM output.
     *
     * audio_samples.mem contains 60,000 lines.
     * ROM depth is 65,536 locations.
     * Extra locations are not played.
     */

    wire [15:0] rom_dout;

    xpm_memory_sprom #(
        .ADDR_WIDTH_A(ADDR_WIDTH),
        .AUTO_SLEEP_TIME(0),
        .CASCADE_HEIGHT(0),
        .ECC_MODE("no_ecc"),
        .MEMORY_INIT_FILE("audio_samples.mem"),
        .MEMORY_INIT_PARAM("0"),
        .MEMORY_OPTIMIZATION("true"),
        .MEMORY_PRIMITIVE("block"),
        .MEMORY_SIZE(ROM_DEPTH * 16),
        .MESSAGE_CONTROL(0),
        .READ_DATA_WIDTH_A(16),
        .READ_LATENCY_A(2),
        .READ_RESET_VALUE_A("0"),
        .RST_MODE_A("SYNC"),
        .SIM_ASSERT_CHK(0),
        .USE_MEM_INIT(1),
        .WAKEUP_TIME("disable_sleep")
    ) sample_rom_inst (
        .douta(rom_dout),
        .addra(sample_index),
        .clka(audio_clk),
        .ena(1'b1),
        .injectdbiterra(1'b0),
        .injectsbiterra(1'b0),
        .regcea(1'b1),
        .rsta(reset),
        .sleep(1'b0),
        .dbiterra(),
        .sbiterra()
    );

    /*
     * Clock relationship:
     *
     * audio_clk = 12.288 MHz
     *
     * BCLK:
     * 12.288 MHz / 4 = 3.072 MHz
     *
     * LRCLK:
     * 3.072 MHz / 64 = 48 kHz
     *
     * One stereo frame = 64 BCLK bits:
     * 32 bits left channel
     * 32 bits right channel
     */

    reg [1:0]  mclk_div;
    reg [5:0]  frame_bit;

    reg [ADDR_WIDTH-1:0] sample_index;
    reg [ADDR_WIDTH-1:0] next_sample_index;

    reg [15:0] active_sample;
    reg [5:0]  next_bit;

    /*
     * sample_repeat_count:
     *
     * 0,1,2,3 = keep same 12 kHz sample
     * then advance to the next sample
     */

    reg [1:0] sample_repeat_count;

    always @(posedge audio_clk) begin
        if (reset) begin
            mclk_div            <= 2'd0;
            BCLK_O              <= 1'b0;
            LRCLK_O             <= 1'b0;
            SDATA_O             <= 1'b0;
            frame_bit           <= 6'd0;
            sample_index        <= {ADDR_WIDTH{1'b0}};
            active_sample       <= 16'd0;
            sample_repeat_count <= 2'd0;
        end else begin

            /*
             * Generate BCLK.
             *
             * Toggle BCLK every 2 audio_clk cycles.
             * Full BCLK period = 4 audio_clk cycles.
             */

            if (mclk_div == 2'd1) begin
                mclk_div <= 2'd0;
                BCLK_O   <= ~BCLK_O;

                /*
                 * Update serial data on falling edge of BCLK.
                 *
                 * Since BCLK_O is the old value inside this always block,
                 * BCLK_O == 1 means this edge changes BCLK from 1 to 0.
                 */

                if (BCLK_O == 1'b1) begin

                    /*
                     * End of one 64-bit stereo frame.
                     *
                     * The codec receives frames at 48 kHz.
                     * The stored audio is 12 kHz.
                     * Therefore, each stored sample is reused for 4 frames.
                     */

                    if (frame_bit == 6'd63) begin
                        next_bit  = 6'd0;
                        frame_bit <= 6'd0;

                        /*
                         * Load current ROM output into active_sample.
                         *
                         * xpm_memory_sprom has read latency.
                         * sample_index changes only once every 4 audio frames,
                         * so rom_dout has plenty of time to become valid.
                         */

                        active_sample <= rom_dout;

                        if (sample_repeat_count == SAMPLE_REPEAT - 1) begin
                            sample_repeat_count <= 2'd0;

                            if (sample_index == SAMPLE_COUNT - 1) begin
                                next_sample_index = {ADDR_WIDTH{1'b0}};
                            end else begin
                                next_sample_index = sample_index + 1'b1;
                            end

                            sample_index <= next_sample_index;
                        end else begin
                            sample_repeat_count <= sample_repeat_count + 1'b1;
                        end

                    end else begin
                        next_bit  = frame_bit + 1'b1;
                        frame_bit <= next_bit;
                    end

                    /*
                     * LRCLK:
                     * 0 = left channel
                     * 1 = right channel
                     */

                    LRCLK_O <= (next_bit >= 6'd32);

                    /*
                     * I2S-style one-bit delay:
                     *
                     * Left channel:
                     *   bit 0 is transition/dummy
                     *   bits 1..16 carry active_sample[15..0]
                     *
                     * Right channel:
                     *   bit 32 is transition/dummy
                     *   bits 33..48 carry active_sample[15..0]
                     *
                     * Same mono sample is sent to both left and right.
                     */

                    if ((next_bit >= 6'd1) && (next_bit <= 6'd16)) begin
                        SDATA_O <= active_sample[6'd16 - next_bit];
                    end else if ((next_bit >= 6'd33) && (next_bit <= 6'd48)) begin
                        SDATA_O <= active_sample[6'd48 - next_bit];
                    end else begin
                        SDATA_O <= 1'b0;
                    end
                end

            end else begin
                mclk_div <= mclk_div + 1'b1;
            end
        end
    end

endmodule
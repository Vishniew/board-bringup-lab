`timescale 1ns / 1ps

module audio_stream_i2s_player (
    input  wire        audio_clk,      // 12.288 MHz audio master clock
    input  wire        reset,          // active-high reset

    input  wire [15:0] sample_data,    // signed 16-bit PCM sample from AXI GPIO
    input  wire        sample_toggle,  // toggled by ARM whenever a new sample is written

    input  wire        SDATA_I,        // codec ADC data, unused

    output reg         BCLK_O,         // 3.072 MHz bit clock
    output reg         LRCLK_O,        // 48 kHz left/right clock
    output reg         SDATA_O         // serial audio data to codec
);

    /*
     * audio_clk = 12.288 MHz
     * BCLK      = 12.288 MHz / 4 = 3.072 MHz
     * LRCLK     = 3.072 MHz / 64 = 48 kHz
     *
     * SONG.WAV is 12 kHz mono 16-bit PCM.
     * ARM sends one sample at 12 kHz through AXI GPIO.
     * This module outputs 48 kHz I2S.
     */

    reg [1:0] mclk_div;
    reg [5:0] frame_bit;

    reg signed [15:0] active_sample;
    reg signed [15:0] pending_sample;
    reg               pending_valid;

    /*
     * AXI GPIO is in the 100 MHz PS/AXI clock domain.
     * This module is in the 12.288 MHz audio clock domain.
     *
     * Synchronize both:
     *   - sample_toggle
     *   - sample_data bus
     *
     * Software rule:
     * ARM writes sample_data first, then toggles sample_toggle.
     * sample_data remains stable until the next sample period.
     */

    (* ASYNC_REG = "TRUE" *) reg        sample_toggle_meta;
    (* ASYNC_REG = "TRUE" *) reg        sample_toggle_sync;
    reg                                 sample_toggle_prev;

    (* ASYNC_REG = "TRUE" *) reg [15:0] sample_data_meta;
    (* ASYNC_REG = "TRUE" *) reg [15:0] sample_data_sync;

    wire new_sample_event;
    assign new_sample_event = (sample_toggle_sync != sample_toggle_prev);

    reg [5:0] next_bit;

    always @(posedge audio_clk) begin
        if (reset) begin
            mclk_div           <= 2'd0;
            frame_bit          <= 6'd0;

            BCLK_O             <= 1'b0;
            LRCLK_O            <= 1'b0;
            SDATA_O            <= 1'b0;

            active_sample      <= 16'sd0;
            pending_sample     <= 16'sd0;
            pending_valid      <= 1'b0;

            sample_toggle_meta <= 1'b0;
            sample_toggle_sync <= 1'b0;
            sample_toggle_prev <= 1'b0;

            sample_data_meta   <= 16'd0;
            sample_data_sync   <= 16'd0;
        end else begin
            /*
             * CDC synchronizers.
             */
            sample_toggle_meta <= sample_toggle;
            sample_toggle_sync <= sample_toggle_meta;
            sample_toggle_prev <= sample_toggle_sync;

            sample_data_meta   <= sample_data;
            sample_data_sync   <= sample_data_meta;

            /*
             * If a new sample arrives, store it as pending.
             * It will be loaded into active_sample only at a frame boundary.
             */
            if (new_sample_event) begin
                pending_sample <= sample_data_sync;
                pending_valid  <= 1'b1;
            end

            /*
             * Generate BCLK.
             * 12.288 MHz / 4 = 3.072 MHz
             */
            if (mclk_div == 2'd1) begin
                mclk_div <= 2'd0;
                BCLK_O   <= ~BCLK_O;

                /*
                 * Update serial audio data on falling edge of BCLK.
                 */
                if (BCLK_O == 1'b1) begin
                    if (frame_bit == 6'd63) begin
                        next_bit  = 6'd0;
                        frame_bit <= 6'd0;

                        /*
                         * Load new sample only at stereo frame boundary.
                         */
                        if (pending_valid) begin
                            active_sample  <= pending_sample;
                            pending_valid  <= 1'b0;
                        end
                    end else begin
                        next_bit  = frame_bit + 6'd1;
                        frame_bit <= next_bit;
                    end

                    /*
                     * LRCLK low  = left channel
                     * LRCLK high = right channel
                     */
                    LRCLK_O <= (next_bit >= 6'd32);

                    /*
                     * I2S-style one-bit delay.
                     * Same mono sample is sent to left and right channels.
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
                mclk_div <= mclk_div + 2'd1;
            end
        end
    end

endmodule
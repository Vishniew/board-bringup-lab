`timescale 1ns / 1ps

module audio_tone_i2s (
    input  wire audio_clk,   // 12.288 MHz audio master clock
    input  wire reset,       // active-high reset

    input  wire SDATA_I,     // codec ADC data, unused in this playback-only project

    output reg  BCLK_O,      // I2S bit clock: 3.072 MHz
    output reg  LRCLK_O,     // I2S left/right clock: 48 kHz
    output reg  SDATA_O      // I2S DAC data to codec
);

    // 12.288 MHz / 4 = 3.072 MHz BCLK
    // 3.072 MHz / 64 = 48 kHz LRCLK
    // 48 samples per sine period at 48 kHz = 1 kHz tone

    reg [1:0] mclk_div;
    reg [5:0] frame_bit;
    reg [5:0] sample_index;
    reg signed [15:0] active_sample;

    reg [5:0] next_bit;
    reg [5:0] next_sample_index;

    function signed [15:0] sine_sample;
        input [5:0] idx;
        begin
            case (idx)
                6'd0:  sine_sample = 16'sd0;
                6'd1:  sine_sample = 16'sd2088;
                6'd2:  sine_sample = 16'sd4141;
                6'd3:  sine_sample = 16'sd6123;
                6'd4:  sine_sample = 16'sd8000;
                6'd5:  sine_sample = 16'sd9740;
                6'd6:  sine_sample = 16'sd11314;
                6'd7:  sine_sample = 16'sd12694;
                6'd8:  sine_sample = 16'sd13856;
                6'd9:  sine_sample = 16'sd14782;
                6'd10: sine_sample = 16'sd15455;
                6'd11: sine_sample = 16'sd15863;
                6'd12: sine_sample = 16'sd16000;
                6'd13: sine_sample = 16'sd15863;
                6'd14: sine_sample = 16'sd15455;
                6'd15: sine_sample = 16'sd14782;
                6'd16: sine_sample = 16'sd13856;
                6'd17: sine_sample = 16'sd12694;
                6'd18: sine_sample = 16'sd11314;
                6'd19: sine_sample = 16'sd9740;
                6'd20: sine_sample = 16'sd8000;
                6'd21: sine_sample = 16'sd6123;
                6'd22: sine_sample = 16'sd4141;
                6'd23: sine_sample = 16'sd2088;
                6'd24: sine_sample = 16'sd0;
                6'd25: sine_sample = -16'sd2088;
                6'd26: sine_sample = -16'sd4141;
                6'd27: sine_sample = -16'sd6123;
                6'd28: sine_sample = -16'sd8000;
                6'd29: sine_sample = -16'sd9740;
                6'd30: sine_sample = -16'sd11314;
                6'd31: sine_sample = -16'sd12694;
                6'd32: sine_sample = -16'sd13856;
                6'd33: sine_sample = -16'sd14782;
                6'd34: sine_sample = -16'sd15455;
                6'd35: sine_sample = -16'sd15863;
                6'd36: sine_sample = -16'sd16000;
                6'd37: sine_sample = -16'sd15863;
                6'd38: sine_sample = -16'sd15455;
                6'd39: sine_sample = -16'sd14782;
                6'd40: sine_sample = -16'sd13856;
                6'd41: sine_sample = -16'sd12694;
                6'd42: sine_sample = -16'sd11314;
                6'd43: sine_sample = -16'sd9740;
                6'd44: sine_sample = -16'sd8000;
                6'd45: sine_sample = -16'sd6123;
                6'd46: sine_sample = -16'sd4141;
                6'd47: sine_sample = -16'sd2088;
                default: sine_sample = 16'sd0;
            endcase
        end
    endfunction

    always @(posedge audio_clk) begin
        if (reset) begin
            mclk_div     <= 2'd0;
            BCLK_O       <= 1'b0;
            LRCLK_O      <= 1'b0;
            SDATA_O      <= 1'b0;
            frame_bit    <= 6'd0;
            sample_index <= 6'd0;
            active_sample <= 16'sd0;
        end else begin
            if (mclk_div == 2'd1) begin
                mclk_div <= 2'd0;
                BCLK_O   <= ~BCLK_O;

                // Update data on falling edge of BCLK.
                // Codec samples data on the opposite edge.
                if (BCLK_O == 1'b1) begin
                    if (frame_bit == 6'd63) begin
                        next_bit = 6'd0;

                        if (sample_index == 6'd47)
                            next_sample_index = 6'd0;
                        else
                            next_sample_index = sample_index + 6'd1;

                        sample_index  <= next_sample_index;
                        active_sample <= sine_sample(next_sample_index);
                        frame_bit     <= 6'd0;
                    end else begin
                        next_bit = frame_bit + 6'd1;
                        frame_bit <= next_bit;
                    end

                    // LRCLK low = left channel, high = right channel
                    LRCLK_O <= (next_bit >= 6'd32);

                    // I2S-style one-bit delay:
                    // bit 0 and bit 32 are dummy/transition bits.
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
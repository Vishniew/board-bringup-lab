module timer (
    input wire clk,
    input wire btn_reset,
    output wire [7:0] an, 
    output reg [6:0] seg  
);

    // ==========================================
    // 1. CLOCK DIVIDERS (Creating Time)
    // ==========================================
    
    reg [26:0] count_1Hz = 0;
    reg tick_1Hz = 0;

    always @(posedge clk) begin
        // NEW: If button is pressed, reset the time counter
        if (btn_reset) begin
            count_1Hz <= 0;
            tick_1Hz <= 0;
        end else if (count_1Hz == 99_999_999) begin
            count_1Hz <= 0;
            tick_1Hz <= 1;
        end else begin
            count_1Hz <= count_1Hz + 1;
            tick_1Hz <= 0;
        end
    end

    reg [16:0] count_1kHz = 0;
    reg tick_1kHz = 0;

    always @(posedge clk) begin
        if (btn_reset) begin
            count_1kHz <= 0;
            tick_1kHz <= 0;
        end else if (count_1kHz == 99_999) begin
            count_1kHz <= 0;
            tick_1kHz <= 1;
        end else begin
            count_1kHz <= count_1kHz + 1;
            tick_1kHz <= 0;
        end
    end

    // ==========================================
    // 2. THE BCD COUNTERS (0 to 9999)
    // ==========================================
    
    reg [3:0] ones = 0;
    reg [3:0] tens = 0;
    reg [3:0] hundreds = 0;
    reg [3:0] thousands = 0;

    always @(posedge clk) begin
        // NEW: If button is pressed, clear all stopwatch digits
        if (btn_reset) begin
            ones <= 0;
            tens <= 0;
            hundreds <= 0;
            thousands <= 0;
        end else if (tick_1Hz) begin
            if (ones == 9) begin
                ones <= 0;
                if (tens == 9) begin
                    tens <= 0;
                    if (hundreds == 9) begin
                        hundreds <= 0;
                        if (thousands == 9) begin
                            thousands <= 0;
                        end else begin
                            thousands <= thousands + 1;
                        end
                    end else begin
                        hundreds <= hundreds + 1;
                    end
                end else begin
                    tens <= tens + 1;
                end
            end else begin
                ones <= ones + 1;
            end
        end
    end

    // ==========================================
    // 3. THE MULTIPLEXER (The Visual Illusion)
    // ==========================================
    
    reg [1:0] digit_select = 0; 

    always @(posedge clk) begin
        if (tick_1kHz) begin
            digit_select <= digit_select + 1;
        end
    end

    reg [7:0] anode_reg;
    assign an = anode_reg;

    always @(*) begin
        case(digit_select)
            2'b00: anode_reg = 8'b1111_1110; 
            2'b01: anode_reg = 8'b1111_1101; 
            2'b10: anode_reg = 8'b1111_1011; 
            2'b11: anode_reg = 8'b1111_0111; 
        endcase
    end

    reg [3:0] current_digit;

    always @(*) begin
        case(digit_select)
            2'b00: current_digit = ones;
            2'b01: current_digit = tens;
            2'b10: current_digit = hundreds;
            2'b11: current_digit = thousands;
        endcase
    end

    always @(*) begin
        case(current_digit)
            4'd0: seg = 7'b1000000;
            4'd1: seg = 7'b1111001;
            4'd2: seg = 7'b0100100;
            4'd3: seg = 7'b0110000;
            4'd4: seg = 7'b0011001;
            4'd5: seg = 7'b0010010;
            4'd6: seg = 7'b0000010;
            4'd7: seg = 7'b1111000;
            4'd8: seg = 7'b0000000;
            4'd9: seg = 7'b0010000;
            default: seg = 7'b1111111; 
        endcase
    end

endmodule
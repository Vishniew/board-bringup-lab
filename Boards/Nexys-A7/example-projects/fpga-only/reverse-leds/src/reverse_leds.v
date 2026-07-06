module reverse_leds (
    input wire [15:0] sw,
    output wire [15:0] led
);

    // Physically cross-wiring the ribbon cable bit-by-bit
    assign led[15] = sw[0];
    assign led[14] = sw[1];
    assign led[13] = sw[2];
    assign led[12] = sw[3];
    assign led[11] = sw[4];
    assign led[10] = sw[5];
    assign led[9]  = sw[6];
    assign led[8]  = sw[7];
    assign led[7]  = sw[8];
    assign led[6]  = sw[9];
    assign led[5]  = sw[10];
    assign led[4]  = sw[11];
    assign led[3]  = sw[12];
    assign led[2]  = sw[13];
    assign led[1]  = sw[14];
    assign led[0]  = sw[15];

endmodule
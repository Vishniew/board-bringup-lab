module led_blink (
    input  wire clk,
    output wire led
);

    reg [26:0] counter = 0;

    always @(posedge clk) begin
        counter <= counter + 1;
    end

    assign led = counter[24];

endmodule
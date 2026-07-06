module led_counter (
    input  wire       clk,
    output wire [7:0] led
);

    reg [31:0] counter = 32'd0;

    always @(posedge clk) begin
        counter <= counter + 1;
    end

    assign led = counter[31:24];

endmodule
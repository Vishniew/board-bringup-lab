module top_switches_leds (
    input  wire [3:0] btn,
    output wire [3:0] led
);

    assign led = btn;

endmodule
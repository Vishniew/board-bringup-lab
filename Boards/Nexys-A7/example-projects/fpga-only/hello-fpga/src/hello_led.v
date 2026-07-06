module hello_led (
    input wire switch_a,
    input wire switch_b,
    output wire led_out
);

    // The '&' symbol tells the synthesizer to physically build an AND gate
    assign led_out = switch_a & switch_b;

endmodule
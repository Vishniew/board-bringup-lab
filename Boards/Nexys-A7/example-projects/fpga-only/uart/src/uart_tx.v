module uart_tx (
    input wire clk,
    input wire tx_start,      // A trigger wire to tell the FSM to begin sending
    input wire [7:0] data_in, // The 8-bit vector holding the character we want to send
    output reg tx_out = 1,    // The physical wire connected to the FTDI chip
    output reg tx_ready = 1   // A status wire to tell our other circuits "I am not busy"
);

    // ==========================================
    // 1. THE BAUD RATE GENERATOR (115,200 bits per second)
    // ==========================================
    // Math: 100,000,000 Hz / 115,200 Baud = 868 clock cycles per bit
    
    reg [9:0] baud_counter = 0;
    reg baud_tick = 0;

    always @(posedge clk) begin
        if (baud_counter == 867) begin
            baud_counter <= 0;
            baud_tick <= 1;
        end else begin
            baud_counter <= baud_counter + 1;
            baud_tick <= 0;
        end
    end

    // ==========================================
    // 2. THE FINITE STATE MACHINE (FSM)
    // ==========================================
    
    reg [2:0] state = 0;          // The current state of the brain
    reg [2:0] bit_index = 0;      // Keeps track of which of the 8 bits we are sending
    reg [7:0] shift_reg = 0;      // The physical memory holding our data

    // These act as highly readable labels for our states
    localparam IDLE = 0;
    localparam START_BIT = 1;
    localparam DATA_BITS = 2;
    localparam STOP_BIT = 3;

    always @(posedge clk) begin
        case (state)
            IDLE: begin
                tx_out <= 1;      // UART protocol dictates the line idles at 3.3V (HIGH)
                tx_ready <= 1;    // We are ready to accept new data
                
                if (tx_start == 1) begin
                    shift_reg <= data_in; // Physically latch the input data into memory
                    tx_ready <= 0;        // Lock the turnstile, we are now busy
                    state <= START_BIT;   // Move the FSM to the next physical state
                end
            end

            START_BIT: begin
                if (baud_tick == 1) begin
                    tx_out <= 0;          // Drive the line to 0V to wake up the receiving laptop
                    state <= DATA_BITS;
                    bit_index <= 0;
                end
            end

            DATA_BITS: begin
                if (baud_tick == 1) begin
                    tx_out <= shift_reg[0];      // Connect the right-most bit directly to the output wire
                    shift_reg <= shift_reg >> 1; // Physically shift all bits in memory one spot to the right
                    
                    if (bit_index == 7) begin
                        state <= STOP_BIT;       // If we've sent all 8 bits, move on
                    end else begin
                        bit_index <= bit_index + 1;
                    end
                end
            end

            STOP_BIT: begin
                if (baud_tick == 1) begin
                    tx_out <= 1;          // Drive line HIGH for the mandatory stop period
                    state <= IDLE;        // Operation complete, return to sleep
                end
            end
        endcase
    end

endmodule
module top (
    input wire clk,
    input wire btn_send,    
    output wire tx_out      
);

    // ==========================================
    // 1. INSTANTIATING THE ENGINE
    // ==========================================
    
    reg [7:0] current_char = 0;
    reg trigger_tx = 0;
    wire engine_ready;

    uart_tx my_transmitter (
        .clk(clk),
        .tx_start(trigger_tx),     
        .data_in(current_char),    
        .tx_out(tx_out),           
        .tx_ready(engine_ready)    
    );

    // ==========================================
    // 2. THE MEMORY ARRAY
    // ==========================================
    
    reg [7:0] message [0:6];
    
    initial begin
        message[0] = 8'h48; // 'H'
        message[1] = 8'h45; // 'E'
        message[2] = 8'h4C; // 'L'
        message[3] = 8'h4C; // 'L'
        message[4] = 8'h4F; // 'O'
        message[5] = 8'h0D; // '\r'
        message[6] = 8'h0A; // '\n'
    end

    // ==========================================
    // 3. THE DEBOUNCER (Upgraded to 50ms)
    // ==========================================
    
    // Expanded the register to 23 bits to hold a larger number
    reg [22:0] debounce_cnt = 0;
    reg btn_clean = 0;

    always @(posedge clk) begin
        if (btn_send == btn_clean) begin
            debounce_cnt <= 0;
        end else begin
            debounce_cnt <= debounce_cnt + 1;
            // Count to 5,000,000 (50 milliseconds at 100MHz)
            if (debounce_cnt == 5_000_000) begin
                btn_clean <= btn_send;
                debounce_cnt <= 0;
            end
        end
    end

    // ==========================================
    // 4. THE ORCHESTRATOR FSM (With Handshake)
    // ==========================================
    
    reg [1:0] state = 0;
    reg [2:0] char_index = 0;
    
    localparam WAIT_FOR_BTN = 0;
    localparam LOAD_CHAR    = 1;
    localparam WAIT_ACK     = 2; // NEW: Wait for UART to acknowledge
    localparam WAIT_DONE    = 3; // NEW: Wait for UART to finish

    reg btn_last = 0;
    wire btn_pressed = (btn_clean == 1) && (btn_last == 0);

    always @(posedge clk) begin
        btn_last <= btn_clean; 

        case (state)
            WAIT_FOR_BTN: begin
                trigger_tx <= 0;
                char_index <= 0; 
                
                if (btn_pressed) begin
                    state <= LOAD_CHAR; 
                end
            end

            LOAD_CHAR: begin
                current_char <= message[char_index]; 
                trigger_tx <= 1;       // Press the engine start button
                state <= WAIT_ACK;     // Move to the Acknowledge state
            end

            WAIT_ACK: begin
                // We freeze here. We will not proceed until the UART 
                // physically pulls the engine_ready wire to 0V.
                if (engine_ready == 0) begin
                    trigger_tx <= 0;   // Release the start button
                    state <= WAIT_DONE; // Now we wait for it to finish
                end
            end

            WAIT_DONE: begin
                // The UART is busy serializing. We wait here for the 
                // 86-microsecond transmission to end and the wire to go HIGH.
                if (engine_ready == 1) begin
                    if (char_index == 6) begin
                        state <= WAIT_FOR_BTN; // Message complete, sleep.
                    end else begin
                        char_index <= char_index + 1; 
                        state <= LOAD_CHAR;    // Load the next character      
                    end
                end
            end
        endcase
    end

endmodule
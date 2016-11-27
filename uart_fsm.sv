module rx_state(input logic sck,
				input logic bck,
				input logic rx,
				output logic [7:0] data);
	
	logic [3:0] baudCount;          // Counting from 0 - 8, one extra bit for overflow
	always_ff @(posedge sck)        // Counter for baudCount
		if (state == S2)  baudCount <= baudCount+1;
		else              baudCount <= 0;

	// State transition
	// S0 : IDLE state, wait for RX to be low
	// S1 : START state, RX sends the starting bit
	// S2 : DATA state, receive and store the data
	// S3 : END state, receive the end bit from rx
	typedef enum logic [1:0] {S0 = 2'b00, S1 = 2'b01, S2 = 2'b10, S3 = 2'b11} statetype; 
	statetype state, nextstate;
	always_ff @(posedge bck, posedge reset)
		if (reset) state <= S0;
		else state <= nextstate;

	// Combination logic for state transition
	always_comb
		case (state)
			// If rx is low, go to next state
			S0: if (rx == 0)  nextstate = S1;
				else          nextstate = S0;
			// Directly go to data state
			S1: nextstate = S2;
			// Wait for baudCount to overflow (i.e. baudCount >= 8)
			S2: if (baudCount[3])   nextstate = S3;
			    else                nextstate = S2;
			// Go to IDLE state
			S3: nextstate = S0;
			default: nextstate = S0;
		endcase

	// Use a shift register to access and store the data
	assign data[baudCount[2:0]] = (state == S2) ? rx : 0;

endmodule
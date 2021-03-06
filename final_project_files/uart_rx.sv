// Wenkai Qin, Jack Yang
// E155 Fall 2016

// uart_rx.sv
// Implements a UART receiver. 1 start bit, 1 stop bit. Takes in async data, outputs
// data on an 8-bit register.

module uart_rx(input  logic clk,
					 input  logic reset,
					 input  logic rx,
					 output logic [7:0] data);
	
	logic read_en, count_en, out_en;
	logic [7:0] tdata;
	logic [7:0] count;
	logic [3:0] bit_c;          // Counting from 0 - 8, one extra bit for overflow

	// State transition
	// S0 : IDLE state, wait for RX to be low
	// S1 : START state, RX sends the starting bit
	// S2 : DATA state, receive and store the data
	// S3 : END state, receive the end bit from rx
	typedef enum logic [1:0] {S0 = 2'b00, S1 = 2'b01, S2 = 2'b10, S3 = 2'b11} statetype; 
	statetype state, nextstate;
	
	always_ff @(posedge clk) begin
		if(reset) begin
			state <= S0;
			count <= 8'b0;
			tdata <= 8'b0;
			data <= 8'b0;

		end else begin
			state <= nextstate;
			
			if(read_en)
				tdata <= {rx, tdata[7:1]};
			    // tdata[bit_c] <= rx;
			if(count_en)
				count <= count + 8'b1;
			else
				count <= 8'b0;

			if(out_en)
				data <= tdata;

		end
	end
		
	// Combination logic for state transition
	always_comb
		case(state) 	
			// If rx is low, go to next state
			S0: if(!rx) nextstate = S1;
				else	nextstate = S0;

			S1: if(count==7) nextstate = S2;
				else		 nextstate = S1;
			
			S2: if(read_en && bit_c >= 7) nextstate = S3;
			    else					  nextstate = S2;
			
			S3: if(read_en && bit_c >= 8) nextstate = S0;
				 else								nextstate = S3;

				 
			default: nextstate = S0;

		endcase
	
	assign read_en   = (((count-7) % 16 == 0) && count > 15);
	assign count_en  = (state != S0);
	assign out_en    = (state == S3);
	assign bit_c	  = (count-8) / 16;
	
endmodule


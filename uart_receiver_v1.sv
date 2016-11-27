module testbench();
	logic clk, reset, rx;
	logic [7:0] data;
	
	finalProject fp0(clk, reset, rx, data);
	
	initial
		forever begin
			clk = 1'b0; #1;
			clk = 1'b1; #1;
		end
		
	initial begin
		rx = 1'b1;
		reset = 1'b1; #20; reset = 1'b0;
		#100;
		rx = 1'b0; #64;
		
		rx = 1'b1; #64;
		rx = 1'b0; #64;
		rx = 1'b1; #64;
		rx = 1'b0; #64;
		rx = 1'b1; #64;
		rx = 1'b0; #64;
		rx = 1'b1; #64;
		rx = 1'b1; #64;

	end
endmodule

module finalProject(input  logic clk,
						  input  logic reset,
						  input  logic rx,
						  output logic [7:0] data);
	
	uart_rx dut(clk, reset, rx, data, test);
	
endmodule

module uart_rx(input  logic clk,
					input  logic reset,
               input  logic rx,
					output logic [7:0] data,
					output logic test);
	
	logic bck, sck, valid;
	clk_gen cg0(clk, reset, bck, sck);
	sampler s0(sck, reset, rx, valid, srx);
	rx_state fsm0(bck, reset, rx, data);
			
endmodule

module sampler(input  logic sck,
					input  logic reset,
					input  logic rx,
					output logic valid,
					output logic out);

	logic [3:0] sck_c;
	logic [2:0] cbit_c;
	logic [4:0] capture;
	
	always_ff@(posedge sck, posedge reset) begin
		
		sck_c <= sck_c + 4'b1;
		
		if(reset) begin
			sck_c <= 4'hf;
			capture <= 5'b01000;
		end else if(cbit_c == 5) begin
			cbit_c <= 3'b0;
			out <= capture[2];
			valid <= (&capture || &(~capture));
		end else if(sck_c <= 2 || sck_c >= 13) begin
			capture[cbit_c] <= rx;
			cbit_c <= cbit_c + 3'b1;
		end
	end
			
endmodule

module rx_state(input  logic bck,
					 input  logic reset,
					 input  logic rx,
					 output logic [7:0] data);
	
	logic [7:0] tempdata;
	logic [2:0] bitCount;          // Counting from 0 - 8, one extra bit for overflow

	// State transition
	// S0 : IDLE state, wait for RX to be low
	// S1 : START state, RX sends the starting bit
	// S2 : DATA state, receive and store the data
	// S3 : END state, receive the end bit from rx
	typedef enum logic [1:0] {S0 = 2'b00, S1 = 2'b01, S2 = 2'b10, S3 = 2'b11} statetype; 
	statetype state, nextstate;
	
	always_ff @(posedge bck)        // Counter for bitCount
		if (state == S1 || state == S2)  bitCount <= bitCount+3'b1;
		else              bitCount <= 0;
		
	always_ff @(posedge bck, posedge reset) begin
		if (reset) state <= S0;
		else if(state == S1 || state == S2) begin
			tempdata <= {rx, tempdata[7:1]};
			state <= nextstate;
		end else if(state == S3) begin
			data <= tempdata;
			state <= nextstate;
		end else
			state <= nextstate;
	end
		
	// Combination logic for state transition
	always_comb
		case (state)
			// If rx is low, go to next state
			S0: if (rx == 0)  nextstate = S1;
				else          nextstate = S0;
			// Directly go to data state
			S1: nextstate = S2;
			// Wait for bitCount to overflow (i.e. bitCount >= 8)
			S2: if (bitCount == 7)   nextstate = S3;
			    else                nextstate = S2;
			// Go to IDLE state
			S3: nextstate = S0;
			default: nextstate = S0;
		endcase

	// Use a shift register to access and store the data
//	assign data[bitCount[2:0]] = (state == S2) ? rx : 0;

endmodule

module clk_gen(input  logic clk,
					input  logic reset,
					output logic bck,
               output logic sck);
					

	logic [17:0] counter;
	logic [17:0] increment = 18'b111111;
	always_ff@(posedge clk, posedge reset)	
		if(reset) counter <= 18'b0;
		else counter <= counter + increment;
//	assign bck = counter[17];
//	assign sck = counter[13];
	
	assign bck = counter[4];
	assign sck = counter[0];
					
endmodule
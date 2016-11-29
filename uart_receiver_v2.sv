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

module final_project(input  logic clk,
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
	rx_state fsm0(bck, reset, rx, data);
			
endmodule


module rx_state(input  logic clk,
					 input  logic reset,
					 input  logic rx,
					 output logic [7:0] data);
	
	logic read_en, count_en, out_en;
	logic [7:0] tdata;
	logic [7:0] count;
	logic [2:0] bit_c;          // Counting from 0 - 8, one extra bit for overflow

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
			bit_c <= 3'b0;
			tdata <= 8'b0;
			data <= 8'b0;

		end else begin
			if(read_en)
				tdata <= {rx, tdata[7:1]};
			    // tdata[bit_c] <= rx;
			if(count_en)
				count <= count + 1;
			else
				count <= 8'b0;

			if(out_en)
				data <= tdata;

		end
	end
		
	// Combination logic for state transition
	always_comb begin
		case(state)
			// If rx is low, go to next state
			S0: if(!rx) nextstate = S1;
				else	nextstate = S0;

			S1: if(count==7) nextstate = S2;
				else		 nextstate = S1;
			
			S2: if(read_en && bit_c >= 7) nextstate = S3;
			    else					  nextstate = S2;
			
			S3: nextstate = S0;

			default: nextstate = S0;

		endcase

		read_en   = ((count-7) % 16 == 16);
		count_en  = (state != S0);
		out_en    = (state == S3);

	end



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
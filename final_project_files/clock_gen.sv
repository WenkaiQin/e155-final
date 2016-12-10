// Wenkai Qin, Jack Yang
// E155 Fall 2016

// clk_gen.sv
// Uses a clock input of 40 MHz to generate output clocks bck at 9.6 kHz and 
// sck at 153.6 kHz.

module clk_gen(input  logic clk,
					input  logic reset,
					output logic bck,
               output logic sck);
					

	logic [17:0] counter;
	logic [17:0] increment = 18'b111111;
	always_ff@(posedge clk)	
		if(reset) counter <= 18'b0;
		else counter <= counter + increment;
	assign bck = counter[17];
	assign sck = counter[13];
	
//  For testing with ModelSim.	
//	assign bck = counter[4];
//	assign sck = counter[0];
					
endmodule
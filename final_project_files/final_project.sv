// Wenkai Qin, Jack Yang
// E155 Fall 2016

// final_project.sv
// Top-level module.

// creset and reset for testing with ModelSim.
module final_project(input  logic clk, /*creset, reset,*/
							input  logic rx,
							output logic [7:0] data,
							output logic lpwm, rpwm) ;
	logic bck, sck;
	assign creset = 1'b0;
	assign reset = 1'b0;
	clk_gen clkgenerator(clk, creset, bck, sck);
	uart_rx receiver(sck, reset, rx, data);

	motor_driver ldriver(sck, reset, data[5:4], lpwm);
	motor_driver rdriver(sck, reset, data[3:2], rpwm);
	
endmodule 
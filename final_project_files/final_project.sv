module final_project(input  logic clk, /*creset, reset,*/
							input  logic rx,
							output logic [7:0] data,
							output logic lpwm, rpwm);
	logic sck;
	assign creset = 1'b0;
	assign reset = 1'b0;
	clk_gen clkgenerator(clk, creset, bck, sck);
	uart_rx receiver(sck, reset, rx, data, test1);
	motor_driver ldriver(sck, reset, data[5:4], lpwm);
	motor_driver rdriver(sck, reset, data[3:2], rpwm);
	
endmodule


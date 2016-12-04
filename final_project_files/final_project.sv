module final_project(input  logic clk, creset, reset,
							input  logic rx,
							output logic [7:0] data);
	logic sck, lpwn, rpwm;

	clk_gen clkgenerator(clk, creset, bck, sck);
	uart_rx receiver(sck, reset, rx, data);
	motor_driver ldriver(clk, reset, data[5:4], lpwn);
	motor_driver rdriver(clk, reset, data[3:2], rpwn);
	
endmodule


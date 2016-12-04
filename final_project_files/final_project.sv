module testbench();
	logic clk, creset, reset, rx;
	logic [7:0] data;
	
	final_project fp0(clk, creset, reset, rx, data);
	
	initial
		forever begin
			clk = 1'b0; #1;
			clk = 1'b1; #1;
		end
		
	initial begin
		rx = 1'b1;
		reset = 1'b1; creset = 1'b1;
		#10; creset = 1'b0;
		#10; reset = 1'b0;
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

module final_project(input  logic clk, creset, reset,
							input  logic rx,
							output logic [7:0] data);
	logic sck, lpwn, rpwm;

	clk_gen clkgenerator(clk, creset, bck, sck);
	uart_rx receiver(sck, reset, rx, data);
	motor_driver ldriver(clk, reset, data[5:4], lpwn);
	motor_driver rdriver(clk, reset, data[3:2], rpwn);
	
endmodule


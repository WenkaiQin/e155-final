module testbench0();
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

module testbench1();
	logic clk, reset, instr, pwm;
	
	motor_driver md0(clk, reset, instr, pwm);
	
	initial
		forever begin
			clk = 1'b0; #1;
			clk = 1'b1; #1;
		end
		
	initial begin
		instr = 2'b11;
		reset = 1'b1;
		#10; reset = 1'b0;
	end
		
endmodule
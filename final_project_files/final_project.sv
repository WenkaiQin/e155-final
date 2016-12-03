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

module final_project(input  logic clk,
							input  logic creset,
							input  logic reset,
							input  logic rx,
							output logic [7:0] data);
	
	uart_rx dut(clk, creset, reset, rx, data, test);
	
endmodule


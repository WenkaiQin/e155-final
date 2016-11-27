// spimaster.sv
// Wenkai Qin
// wqin@hmc.edu

module testbench();
	logic clk, reset, sck, miso, mosi, ss, attn;
	logic [7:0] din, dout;
	
	spimaster dut(clk, reset, sck, mosi, mosi, ss, attn, din, dout);
	
	initial
		forever begin
			clk = 1'b0; #2;
			clk = 1'b1; #2;
		end
	
	initial begin
		din = 8'h50;
		reset = 1'b1; #900f;
		reset = 1'b0;
		
		attn = 1'b0; #270;
		attn = 1'b1; #900;
		attn = 1'b0;
	end
	
endmodule

module spimaster(input logic clk, reset,
				 output logic sck,
				 input logic miso,
				 output logic mosi,
				 output logic ss,
				 input logic attn,
				 input logic [7:0] data_in,
				 output logic [7:0] data_out);

	logic test;
	logic [3:0] sck_c;
	logic [4:0] clk_c;
	logic [7:0] data_in_d, data_out_d;

	initial
		clk_c = 5'b0;
	
	always_ff@(posedge clk) begin
		clk_c <= clk_c + 5'b1;
	end
	
	always_ff@(posedge clk_c[4]) begin
		
// SCK IMPLEMENTATION 1
//		if(sck_c == 8 && ~sck) begin
//			sck <= sck;
//			sck_c <= sck_c + 1;
//			data_out <= data_out_d;
//		end else if(sck_c < 9) begin
//			sck <= ~sck;
//			if(~sck)
//				sck_c <= sck_c + 1;
//				
//		end else if(attn) begin
//			sck_c <= 4'b0;
//			data_in_d <= data_in;
//		end

// SCK IMPLEMENTATION 2
//		if(sck && sck_c < 9)
//			sck <= ~sck;
//		else if(~sck && sck_c == 8) begin
//			sck_c <= sck_c + 4'b1;
//			data_out <= data_out_d;
//		end else if(~sck && sck_c < 9) begin
//			sck <= ~sck;
//			sck_c <= sck_c + 4'b1;
//		end else if(attn) begin
//			sck_c <= 4'b0;
//			data_in_d <= data_in;
//		end

// SCK IMPLEMENTATION 3
		if(sck_c < 9)
			if(sck)
				sck <= ~sck;
			else begin
			sck_c <= sck_c + 4'b1;
			if(sck_c == 8) begin
				sck <= sck;
				data_out <= data_out_d;
			end else
				sck <= ~sck;
			end
		else if(attn) begin
			sck_c <= 4'b0;
			data_in_d <= data_in;
		end
					
		if(reset) begin
			sck <= 1'b0;
			sck_c <= 4'h9;
			mosi <= 1'bx;
			data_out <= 8'bx;
		end
	end

	always_ff@(posedge sck)
		data_out_d[8-sck_c] <= miso;
	
	always_ff@(negedge sck, negedge ss)
		mosi <= data_in_d[7-sck_c];

	always_ff@(posedge ss)
		data_out <= data_out;
		
	assign ss = (sck_c == 9);
	assign test = clk_c[4];
endmodule
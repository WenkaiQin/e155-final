module motor_driver(input  logic clk, reset,
					input  logic [1:0] instr,
					output logic pwm);
	
	// Wire for enabling update of the instruction
	logic update_en;
	// 12 bits needed to reach 3072.
	logic [11:0] count;

	always_ff@(posedge clk) begin
		if(reset)
			count <= 12'b0;
		else
			count <= count + 12'b1;

			
endmodule

module instr_dec(input  logic instr,
				 output logic [11:0] target_n);
	always_comb
		case(instr)
			// Appropriate count targets for forward, stop, back.
			// Back: PW = 1.0 ms, 154 cycles
			// Stop: PW = 1.5 ms, 230 cycles
			// Forward: PW = 2.0 ms, 307 cycles
			2'b10: target_n = 11'b10011010;
			2'b11: target_n = 11'b11100110;
			2'b01: target_n = 11'b100110011;
			// Default stop.
			default: target_n = 11'b11100110;
		endcase
endmodule

	
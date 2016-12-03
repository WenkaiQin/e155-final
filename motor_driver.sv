module motor_driver(input  logic clk, reset,
					input  logic [1:0] instr,
					output logic pwm);
	
	// Wire for enabling update of the instruction
	logic update_en;
	// 12 bits needed to reach 3072 for count and target.
	logic [11:0] count, target_n;
	// 2-bit instruction register.
	logic [1:0] instr_d;

	instr_dec id0(instr_d, target_n);

	// Define states:
	// S0: Pulse high. Cannot update instruction.
	// S1: Pulse low. Updating instruction.
	typedef enum logic {S0 = 1'b0, S1 = 1'b1} statetype; 
	statetype state, nextstate;

	always_ff@(posedge clk) begin
		if(reset) begin
			count <= 12'b0;
			state <= S0;
			instr <= 2'b11;
		end else begin
			if(count == 3071)
				count <= 12'b0;
			else
				count <= count + 12'b1;
			state <= nextstate
			if(update_en)
				instr_d <= instr;
		end

	always_comb
		case(state)
			// If we reached the target, transition.
			S0: if(count >= target - 1) nextstate = S1;
				else					nextstate = S0;
			
			S1: if(count >= 3071) nextstate = S0;
				else			  nextstate = S1;

			default: nextstate = S0;

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

	
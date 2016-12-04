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
			instr_d <= 2'b11;
		end else begin
<<<<<<< HEAD
			if(count >= 3071)
=======
			if(count >= 199)
>>>>>>> origin/master
				count <= 12'b0;
			else
				count <= count + 12'b1;
				
			state <= nextstate;
			
			if(update_en)
				instr_d <= instr;
		end
	end

	always_comb
		case(state)
			// If we reached the target, transition.
			S0: if(count >= target_n - 1) nextstate = S1;
				else					nextstate = S0;
			
<<<<<<< HEAD
			S1: if(count >= 3071) nextstate = S0;
=======
			S1: if(count >= 199) nextstate = S0;
>>>>>>> origin/master
				else			  nextstate = S1;

			default: nextstate = S0;
		endcase
		
	assign update_en = (state == S1);
	assign pwm = (state == S0);
<<<<<<< HEAD
	
	//assign instr_d = (state == S1) ? instr : instr_d;
	
=======
>>>>>>> origin/master
endmodule

module instr_dec(input  logic [1:0] instr,
				 output logic [11:0] target_n);
	always_comb
		case(instr)
			// Appropriate count targets for forward, stop, back.
			// Back: PW = 1.0 ms, 154 cycles
			// Stop: PW = 1.5 ms, 230 cycles
			// Forward: PW = 2.0 ms, 307 cycles
			
<<<<<<< HEAD
			2'b10: target_n = 11'b10011010;
			2'b11: target_n = 11'b11100110;
			2'b01: target_n = 11'b100110011;
			
			/*2'b10: target_n = 15;
			2'b10: target_n = 10;
			2'b10: target_n = 20;*/
			// Default stop.
			default: target_n = 11'b11100110;
			// default: target_n = 15;
		endcase
endmodule
=======
			/*2'b10: target_n = 11'b10011010;
			2'b11: target_n = 11'b11100110;
			2'b01: target_n = 11'b100110011;*/
			
			2'b10: target_n = 15;
			2'b10: target_n = 10;
			2'b10: target_n = 20;
			// Default stop.
			//default: target_n = 11'b11100110;
			default: target_n = 15;
		endcase
endmodule
>>>>>>> origin/master

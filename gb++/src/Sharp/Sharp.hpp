#ifndef SHARP_HPP
#define SHARP_HPP

#include <cstdint>
#include <string>

class Memory;

// Some Definitions:
// Word (W) = 8 bits / 1 byte
// Double-word (DW) = 16 bits / 2 bytes
// Signed Word (SW) = 8 bits / 1 byte, signed integer

class Sharp {
	uint16_t SP; // Stack pointer
	uint16_t PC; // Program counter (Instruction pointer)

	// Current variables
	uint16_t CurrOperand;
	uint8_t  CurrCycles;
	uint8_t  CurrArgSize;
	uint8_t  Opcode;
	uint8_t  temp;
	uint8_t  temp2;
	uint16_t temp3;
	uint8_t	 InterruptMasterEnable;

	// 16-bit registers grouped together using anonymous unions and structs
	union {
		uint16_t AF;
		struct {
			uint8_t F;
			uint8_t A;
		};
	};

	union {
		uint16_t BC;
		struct {
			uint8_t C;
			uint8_t B;
		};
	};

	union {
		uint16_t DE;
		struct {
			uint8_t E;
			uint8_t D;
		};
	};

	union {
		uint16_t HL;
		struct {
			uint8_t L;
			uint8_t H;
		};
	};

	// Enums to help extract flags from the F register
	enum SharpFlags {
		z = (1 << 7),
		n = (1 << 6),
		h = (1 << 5),
		c = (1 << 4)
	};

	void SetFlag(SharpFlags flag, bool set);
	uint8_t GetFlag(SharpFlags flag);

	// Helper functions to reduce repeated code
	void DecrementRegister(uint8_t& reg);
	void IncrementRegister(uint8_t& reg);
	void UnsignedAdd(uint8_t& dest, uint8_t src);
	void UnsignedAddCarry(uint8_t& dest, uint8_t src);
	void UnsignedAdd16(uint16_t& dest, uint16_t src);
	void Subtract(uint8_t& dest, uint8_t src);
	void SubtractWithCarry(uint8_t& dest, uint8_t src);
	void RotateLeftCircular(uint8_t& arg);
	void RotateRightCircular(uint8_t& arg);
	void RotateLeft(uint8_t& arg);
	void RotateRight(uint8_t& arg);
	void And(uint8_t arg);
	void Xor(uint8_t arg);
	void Or(uint8_t arg);
 
	// Needed for the logical CPU to address the memory
	Memory* MemoryBus;

	// SM83 Instruction Set

	// Unknown Opcode
	void UNOP();

	// First Row of Table (0x00 - 0x0F)
	void NOP();
	void LD_BC_DW();
	void LD_ADDR_BC_A();
	void INC_BC();
	void INC_B();
	void DEC_B();
	void LD_B_W();
	void RLCA();
	void LD_ADDR_DW_SP();
	void ADD_HL_BC();
	void LD_A_ADDR_BC();
	void DEC_BC();
	void INC_C();
	void DEC_C();
	void LD_C_W();
	void RRCA();

	// Second Row of Table (0x10 - 0x1F)
	void STOP();
	void LD_DE_DW();
	void LD_ADDR_DE_A();
	void INC_DE();
	void INC_D();
	void DEC_D();
	void LD_D_W();
	void RLA();
	void JR_SW();
	void ADD_HL_DE();
	void LD_A_ADDR_DE();
	void DEC_DE();
	void INC_E();
	void DEC_E();
	void LD_E_W();
	void RRA();

	// Third Row of Table (0x20 - 0x2F)
	void JR_NZ_SW();
	void LD_HL_DW();
	void LD_ADDR_HL_PI_A();
	void INC_HL();
	void INC_H();
	void DEC_H();
	void LD_H_W();
	void DAA();
	void JR_Z_SW();
	void ADD_HL_HL();
	void LD_A_ADDR_HL_PI();
	void DEC_HL();
	void INC_L();
	void DEC_L();
	void LD_L_W();
	void CPL();

	// Fourth Row of Table (0x30 - 0x3F)
	void JR_NC_SW();
	void LD_SP_DW();
	void LD_ADDR_HL_PD_A();
	void INC_SP();
	void INC_ADDR_HL();
	void DEC_ADDR_HL();
	void LD_ADDR_HL_W();
	void SCF();
	void JR_C_SW();
	void ADD_HL_SP();
	void LD_A_ADDR_HL_PD();
	void DEC_SP();
	void INC_A();
	void DEC_A();
	void LD_A_W();
	void CCF();

	// Fifth Row of Table (0x40 - 0x4F)
	void LD_B_B();
	void LD_B_C();
	void LD_B_D();
	void LD_B_E();
	void LD_B_H();
	void LD_B_L();
	void LD_B_ADDR_HL();
	void LD_B_A();
	void LD_C_B();
	void LD_C_C();
	void LD_C_D();
	void LD_C_E();
	void LD_C_H();
	void LD_C_L();
	void LD_C_ADDR_HL();
	void LD_C_A();

	// Sixth Row of Table (0x50 - 0x5F)
	void LD_D_B();
	void LD_D_C();
	void LD_D_D();
	void LD_D_E();
	void LD_D_H();
	void LD_D_L();
	void LD_D_ADDR_HL();
	void LD_D_A();
	void LD_E_B();
	void LD_E_C();
	void LD_E_D();
	void LD_E_E();
	void LD_E_H();
	void LD_E_L();
	void LD_E_ADDR_HL();
	void LD_E_A();

	// Seventh Row of Table (0x60 - 0x6F)
	void LD_H_B();
	void LD_H_C();
	void LD_H_D();
	void LD_H_E();
	void LD_H_H();
	void LD_H_L();
	void LD_H_ADDR_HL();
	void LD_H_A();
	void LD_L_B();
	void LD_L_C();
	void LD_L_D();
	void LD_L_E();
	void LD_L_H();
	void LD_L_L();
	void LD_L_ADDR_HL();
	void LD_L_A();

	// Eighth Row of Table (0x70 - 0x7F)
	void LD_ADDR_HL_B();
	void LD_ADDR_HL_C();
	void LD_ADDR_HL_D();
	void LD_ADDR_HL_E();
	void LD_ADDR_HL_H();
	void LD_ADDR_HL_L();
	void HALT();
	void LD_ADDR_HL_A();
	void LD_A_B();
	void LD_A_C();
	void LD_A_D();
	void LD_A_E();
	void LD_A_H();
	void LD_A_L();
	void LD_A_ADDR_HL();
	void LD_A_A();

	// Ninth Row of Table (0x80 - 0x8F)
	void ADD_A_B();
	void ADD_A_C();
	void ADD_A_D();
	void ADD_A_E();
	void ADD_A_H();
	void ADD_A_L();
	void ADD_A_ADDR_HL();
	void ADD_A_A();
	void ADC_A_B();
	void ADC_A_C();
	void ADC_A_D();
	void ADC_A_E();
	void ADC_A_H();
	void ADC_A_L();
	void ADC_A_ADDR_HL();
	void ADC_A_A();
	
	// Tenth Row of Table (0x90 - 0x9F)
	void SUB_B();
	void SUB_C();
	void SUB_D();
	void SUB_E();
	void SUB_H();
	void SUB_L();
	void SUB_ADDR_HL();
	void SUB_A();
	void SBC_A_B();
	void SBC_A_C();
	void SBC_A_D();
	void SBC_A_E();
	void SBC_A_H();
	void SBC_A_L();
	void SBC_A_ADDR_HL();
	void SBC_A_A();

	// Eleventh Row of Table (0xA0 - 0xAF)
	void AND_B();
	void AND_C();
	void AND_D();
	void AND_E();
	void AND_H();
	void AND_L();
	void AND_ADDR_HL();
	void AND_A();
	void XOR_B();
	void XOR_C();
	void XOR_D();
	void XOR_E();
	void XOR_H();
	void XOR_L();
	void XOR_ADDR_HL();
	void XOR_A();

	// Twelfth Row of Table (0xB0 - 0xBF)
	void OR_B();
	void OR_C();
	void OR_D();
	void OR_E();
	void OR_H();
	void OR_L();
	void OR_ADDR_HL();
	void OR_A();
	void CP_B();
	void CP_C();
	void CP_D();
	void CP_E();
	void CP_H();
	void CP_L();
	void CP_ADDR_HL();
	void CP_A();

	// Thirteenth Row of Table (0xC0 - 0xCF)
	void RET_NZ();
	void POP_BC();
	void JP_NZ_ADDR_DW();
	void JP_ADDR_DW();
	void CALL_NZ_ADDR_DW();
	void PUSH_BC();
	void ADD_A_W();
	void RST_00H();
	void RET_Z();
	void RET();
	void JP_Z_ADDR_DW();
	void PREFIX_CB();
	void CALL_Z_ADDR_DW();
	void CALL_ADDR_DW();
	void ADC_A_W();
	void RST_08H();

	// Fourteenth Row of Table (0xD0 - 0xDF)
	void RET_NC();
	void POP_DE();
	void JP_NC_ADDR_DW();
	// Unknown Opcode
	void CALL_NC_ADDR_DW();
	void PUSH_DE();
	void SUB_W();
	void RST_10H();
	void RET_C();
	void RETI();
	void JP_C_ADDR_DW();
	// Unknown Opcode
	void CALL_C_ADDR_DW();
	// Unknown Opcode
	void SBC_A_W();
	void RST_18H();

	// Fifteenth Row of Table (0xE0 - 0xEF)
	void LDH_ADDR_W_A();
	void POP_HL();
	void LD_ADDR_C_A();
	void PUSH_HL();
	void AND_W();
	void RST_20H();
	void ADD_SP_SW();
	void JP_HL();
	void LD_ADDR_DW_A();
	void XOR_W();
	void RST_28H();

	struct SharpInstr {
		uint8_t ArgSize; // Can be 0 words, 1 word, or 2 words
		uint8_t Cycles; // Number of cycles the instructions take
		void (Sharp::*Instruction)(void); // Pointer to a function
	};

	// Holder variable for the current instruction
	SharpInstr DecodedInstr;
	const struct SharpInstr SHARPINSTRS[224] = {
		{0,  4, &Sharp::NOP            }, {2, 12, &Sharp::LD_BC_DW    }, {0,  8, &Sharp::LD_ADDR_BC_A   }, {0,  8, &Sharp::INC_BC	   }, 
		{0,  4, &Sharp::INC_B          }, {0,  4, &Sharp::DEC_B       }, {1,  8, &Sharp::LD_B_W         }, {0,  4, &Sharp::RLCA		   }, 
		{2, 20, &Sharp::LD_ADDR_DW_SP  }, {0,  8, &Sharp::ADD_HL_BC   }, {0,  8, &Sharp::LD_A_ADDR_BC   }, {0,  8, &Sharp::DEC_BC	   },
		{0,  4, &Sharp::INC_C          }, {0,  4, &Sharp::DEC_C       }, {1,  8, &Sharp::LD_C_W         }, {0,  4, &Sharp::RRCA		   },
									  							    										 
		{1,  4, &Sharp::STOP           }, {2, 12, &Sharp::LD_DE_DW    }, {0, 12, &Sharp::LD_ADDR_BC_A   }, {0,  8, &Sharp::INC_DE	   },
		{0,  4, &Sharp::INC_D          }, {0,  4, &Sharp::DEC_D       }, {1,  8, &Sharp::LD_D_W         }, {0,  4, &Sharp::RLA		   },
		{1, 12, &Sharp::JR_SW          }, {0,  8, &Sharp::ADD_HL_DE   }, {0,  8, &Sharp::LD_A_ADDR_DE   }, {0,  8, &Sharp::DEC_DE	   },
		{0,  4, &Sharp::INC_E          }, {0,  4, &Sharp::DEC_E       }, {1,  8, &Sharp::LD_E_W         }, {0,  4, &Sharp::RRA 		   },
									  							    										 
		{1,  8, &Sharp::JR_NZ_SW       }, {2, 12, &Sharp::LD_HL_DW    }, {0,  8, &Sharp::LD_ADDR_HL_PI_A}, {0,  8, &Sharp::INC_HL	   },
		{0,  4, &Sharp::INC_H          }, {0,  4, &Sharp::DEC_H       }, {1,  8, &Sharp::LD_H_W         }, {0,  4, &Sharp::DAA		   },
		{1,  8, &Sharp::JR_Z_SW        }, {0,  8, &Sharp::ADD_HL_HL   }, {0,  8, &Sharp::LD_A_ADDR_HL_PI}, {0,  8, &Sharp::DEC_HL	   },
		{0,  4, &Sharp::INC_L          }, {0,  4, &Sharp::DEC_L       }, {1,  8, &Sharp::LD_L_W         }, {0,  4, &Sharp::CPL	 	   },
									  																		 
		{1,  8, &Sharp::JR_NC_SW       }, {2, 12, &Sharp::LD_SP_DW    }, {0,  8, &Sharp::LD_ADDR_HL_PD_A}, {0,  8, &Sharp::INC_SP      },
		{0, 12, &Sharp::INC_ADDR_HL    }, {0, 12, &Sharp::DEC_ADDR_HL }, {1, 12, &Sharp::LD_ADDR_HL_W   }, {0,  4, &Sharp::SCF         },
		{1,  8, &Sharp::JR_C_SW        }, {0,  8, &Sharp::ADD_HL_SP   }, {0,  8, &Sharp::LD_A_ADDR_HL_PD}, {0,  8, &Sharp::DEC_SP      },
		{0,  4, &Sharp::INC_A          }, {0,  4, &Sharp::DEC_A       }, {1,  8, &Sharp::LD_A_W         }, {0,  4, &Sharp::CCF         },
									  							    										 
		{0,  4, &Sharp::LD_B_B         }, {0,  4, &Sharp::LD_B_C      }, {0,  4, &Sharp::LD_B_D         }, {0,  4, &Sharp::LD_B_E      },
		{0,  4, &Sharp::LD_B_H         }, {0,  4, &Sharp::LD_B_L      }, {0,  8, &Sharp::LD_B_ADDR_HL   }, {0,  4, &Sharp::LD_B_A      },
		{0,  4, &Sharp::LD_C_B         }, {0,  4, &Sharp::LD_C_C      }, {0,  4, &Sharp::LD_C_D         }, {0,  4, &Sharp::LD_C_E      },
		{0,  4, &Sharp::LD_C_H         }, {0,  4, &Sharp::LD_C_L      }, {0,  8, &Sharp::LD_C_ADDR_HL   }, {0,  4, &Sharp::LD_C_A      },
									  							    										 
		{0,  4, &Sharp::LD_D_B         }, {0,  4, &Sharp::LD_D_C      }, {0,  4, &Sharp::LD_D_D         }, {0,  4, &Sharp::LD_D_E      },
		{0,  4, &Sharp::LD_D_H         }, {0,  4, &Sharp::LD_D_L      }, {0,  8, &Sharp::LD_D_ADDR_HL   }, {0,  4, &Sharp::LD_D_A      },
		{0,  4, &Sharp::LD_E_B         }, {0,  4, &Sharp::LD_E_C      }, {0,  4, &Sharp::LD_E_D         }, {0,  4, &Sharp::LD_E_E      },
		{0,  4, &Sharp::LD_E_H         }, {0,  4, &Sharp::LD_E_L      }, {0,  8, &Sharp::LD_E_ADDR_HL   }, {0,  4, &Sharp::LD_E_A      },
									  							    										 
		{0,  4, &Sharp::LD_H_B         }, {0,  4, &Sharp::LD_H_C      }, {0,  4, &Sharp::LD_H_D         }, {0,  4, &Sharp::LD_H_E      },
		{0,  4, &Sharp::LD_H_H         }, {0,  4, &Sharp::LD_H_L      }, {0,  8, &Sharp::LD_H_ADDR_HL   }, {0,  4, &Sharp::LD_H_A      },
		{0,  4, &Sharp::LD_L_B         }, {0,  4, &Sharp::LD_L_C      }, {0,  4, &Sharp::LD_L_D         }, {0,  4, &Sharp::LD_L_E      },
		{0,  4, &Sharp::LD_L_H         }, {0,  4, &Sharp::LD_L_L      }, {0,  8, &Sharp::LD_L_ADDR_HL   }, {0,  4, &Sharp::LD_L_A      },
									  																		 
		{0,  4, &Sharp::LD_ADDR_HL_B   }, {0,  4, &Sharp::LD_ADDR_HL_C}, {0,  4, &Sharp::LD_ADDR_HL_D   }, {0,  4, &Sharp::LD_ADDR_HL_E},
		{0,  4, &Sharp::LD_ADDR_HL_H   }, {0,  4, &Sharp::LD_ADDR_HL_L}, {0,  8, &Sharp::HALT           }, {0,  4, &Sharp::LD_ADDR_HL_A},
		{0,  4, &Sharp::LD_A_B         }, {0,  4, &Sharp::LD_A_C      }, {0,  4, &Sharp::LD_A_D         }, {0,  4, &Sharp::LD_A_E      },
		{0,  4, &Sharp::LD_A_H         }, {0,  4, &Sharp::LD_A_L      }, {0,  8, &Sharp::LD_A_ADDR_HL   }, {0,  4, &Sharp::LD_A_A      },
									  																		 
		{0,  4, &Sharp::ADD_A_B        }, {0,  4, &Sharp::ADD_A_C     }, {0,  4, &Sharp::ADD_A_D        }, {0,  4, &Sharp::ADD_A_E     },
		{0,  4, &Sharp::ADD_A_H        }, {0,  4, &Sharp::ADD_A_L     }, {0,  8, &Sharp::ADD_A_ADDR_HL  }, {0,  4, &Sharp::ADD_A_A     },
		{0,  4, &Sharp::ADC_A_B        }, {0,  4, &Sharp::ADC_A_C     }, {0,  4, &Sharp::ADC_A_D        }, {0,  4, &Sharp::ADC_A_E     },
		{0,  4, &Sharp::ADC_A_H        }, {0,  4, &Sharp::ADC_A_L     }, {0,  8, &Sharp::ADC_A_ADDR_HL  }, {0,  4, &Sharp::ADC_A_A     },
									  																		 
		{0,  4, &Sharp::SUB_B          }, {0,  4, &Sharp::SUB_C       }, {0,  4, &Sharp::SUB_D          }, {0,  4, &Sharp::SUB_E       },
		{0,  4, &Sharp::SUB_H          }, {0,  4, &Sharp::SUB_L       }, {0,  8, &Sharp::SUB_ADDR_HL    }, {0,  4, &Sharp::SUB_A       },
		{0,  4, &Sharp::SBC_A_B        }, {0,  4, &Sharp::SBC_A_C     }, {0,  4, &Sharp::SBC_A_D        }, {0,  4, &Sharp::SBC_A_E     },
		{0,  4, &Sharp::SBC_A_H        }, {0,  4, &Sharp::SBC_A_L     }, {0,  8, &Sharp::SBC_A_ADDR_HL  }, {0,  4, &Sharp::SBC_A_A     },
									  																		 
		{0,  4, &Sharp::AND_B          }, {0,  4, &Sharp::AND_C       }, {0,  4, &Sharp::AND_D          }, {0,  4, &Sharp::AND_E       },
		{0,  4, &Sharp::AND_H          }, {0,  4, &Sharp::AND_L       }, {0,  8, &Sharp::AND_ADDR_HL    }, {0,  4, &Sharp::AND_A       },
		{0,  4, &Sharp::XOR_B          }, {0,  4, &Sharp::XOR_C       }, {0,  4, &Sharp::XOR_D          }, {0,  4, &Sharp::XOR_E       },
		{0,  4, &Sharp::XOR_H          }, {0,  4, &Sharp::XOR_L       }, {0,  8, &Sharp::XOR_ADDR_HL    }, {0,  4, &Sharp::XOR_A       },
									  																		 
		{0,  4, &Sharp::OR_B           }, {0,  4, &Sharp::OR_C        }, {0,  4, &Sharp::OR_D           }, {0,  4, &Sharp::OR_E        },
		{0,  4, &Sharp::OR_H           }, {0,  4, &Sharp::OR_L        }, {0,  8, &Sharp::OR_ADDR_HL     }, {0,  4, &Sharp::OR_A        },
		{0,  4, &Sharp::CP_B           }, {0,  4, &Sharp::CP_C        }, {0,  4, &Sharp::CP_D           }, {0,  4, &Sharp::CP_E        },
		{0,  4, &Sharp::CP_H           }, {0,  4, &Sharp::CP_L        }, {0,  8, &Sharp::CP_ADDR_HL     }, {0,  4, &Sharp::CP_A        },
																											 
		{0,  8, &Sharp::RET_NZ		   }, {0, 12, &Sharp::POP_BC	  }, {2, 12, &Sharp::JP_NZ_ADDR_DW  }, {2, 16, &Sharp::JP_ADDR_DW  },
		{2, 12, &Sharp::CALL_NZ_ADDR_DW}, {0, 16, &Sharp::PUSH_BC     }, {1, 8,&Sharp::ADD_A_W          }, {0, 16, &Sharp::RST_00H     },
		{0,  8, &Sharp::RET_Z		   }, {0, 16, &Sharp::RET         }, {2,12,&Sharp::JP_Z_ADDR_DW     }, {0,  0, &Sharp::PREFIX_CB   },
		{2, 12, &Sharp::CALL_Z_ADDR_DW }, {2, 24, &Sharp::CALL_ADDR_DW}, {1, 8,&Sharp::ADC_A_W          }, {0, 16, &Sharp::RST_08H     },

		{0,  8, &Sharp::RET_NC		   }, {0, 12, &Sharp::POP_DE	  }, {2, 12, &Sharp::JP_NC_ADDR_DW  }, {0,  4, &Sharp::UNOP		   },
		{2, 12, &Sharp::CALL_NC_ADDR_DW}, {0, 16, &Sharp::PUSH_DE	  }, {1,  8, &Sharp::SUB_W			}, {0, 16, &Sharp::RST_10H	   },
		{0,  8, &Sharp::RET_C		   }, {0, 16, &Sharp::RETI		  }, {2, 12, &Sharp::JP_C_ADDR_DW	}, {0,  4, &Sharp::UNOP		   },
		{2, 12, &Sharp::CALL_C_ADDR_DW }, {0,  4, &Sharp::UNOP		  }, {1,  8, &Sharp::SBC_A_W		}, {0, 16, &Sharp::RST_18H	   }
	};	  					 

public:
	bool Suspended;

	Sharp(Memory* _MemoryBus);
	~Sharp();

	void Clock();
};

#endif

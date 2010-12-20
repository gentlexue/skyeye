/*
 * The interface of dynamic compiled mode for ppc simulation
 *
 * 08/22/2010 Michael.Kang (blackfin.kang@gmail.com)
 */
#include <skyeye_dyncom.h>
#include <skyeye_types.h>
#include <skyeye_obj.h>
#include <skyeye.h>

#include "armdefs.h"
#include "arm_translate.h"
#define MAX_REGNUM 15

uint32_t get_end_of_page(uint32 phys_addr){
	const uint32 page_size = 4 * 1024;
	return (phys_addr + page_size) & (~(page_size - 1));
}
/* physical register for arm archtecture */
static void arch_arm_init(cpu_t *cpu, cpu_archinfo_t *info, cpu_archrf_t *rf)
{
	// Basic Information
	info->name = "arm";
	info->full_name = "arm_dyncom";

	// This architecture is biendian, accept whatever the
	// client wants, override other flags.
	info->common_flags &= CPU_FLAG_ENDIAN_MASK;

	info->delay_slots = 0;
	// The byte size is 8bits.
	// The word size is 32bits.
	// The float size is 64bits.
	// The address size is 32bits.
	info->byte_size = 8;
	info->word_size = 32;
	info->float_size = 64;
	info->address_size = 32;
	// There are 16 32-bit GPRs
	info->register_count[CPU_REG_GPR] = MAX_REGNUM;
	info->register_size[CPU_REG_GPR] = info->word_size;
	// There is also 1 extra register to handle PSR.
	//info->register_count[CPU_REG_XR] = PPC_XR_SIZE;
	info->register_count[CPU_REG_XR] = 16;
	info->register_size[CPU_REG_XR] = 32;
	info->register_count[CPU_REG_SPR] = 7;
	info->register_size[CPU_REG_SPR] = 32;
	cpu->redirection = false;

	/* Initilize different register set for different core */
	arm_core_t* core = (arm_core_t*)cpu->cpu_data;
	cpu->rf.pc = &core->pc;
	cpu->rf.grf =(Value*) core->Reg;
	cpu->rf.srf =(Value*) core->Spsr;
#if 0
	cpu->ptr_PC = (Value*)&core->pc;
	cpu->ptr_gpr =(Value**) &core->Reg;
	cpu->ptr_xr = (Value**)&core->Cpsr;
	cpu->in_ptr_xr = (Value**)&core->Spsr;
#endif
	cpu->ptr_N = (Value*)&core->NFlag;
	cpu->ptr_V = (Value*)&core->VFlag;
	cpu->ptr_Z = (Value*)&core->ZFlag;
	cpu->ptr_C = (Value*)&core->CFlag;
}

static void
arch_powerpc_done(cpu_t *cpu)
{
	//free(cpu->rf.grf);
}

static addr_t
arch_arm_get_pc(cpu_t *, void *reg)
{
	//return ((reg_powerpc_t *)reg)->pc;
	return 0;
}

static uint64_t
arch_arm_get_psr(cpu_t *, void *reg)
{
	return 0;
}

static int
arch_arm_get_reg(cpu_t *cpu, void *reg, unsigned reg_no, uint64_t *value)
{
	return (0);
}

static int arch_arm_disasm_instr(cpu_t *cpu, addr_t pc, char* line, unsigned int max_line){
	return 0;
}
static int arch_arm_translate_loop_helper(cpu_t *cpu, addr_t pc, BasicBlock *bb_ret, BasicBlock *bb_next, BasicBlock *bb, BasicBlock *bb_zol_cond){
	return 0;
}
static arch_func_t arm_arch_func = {
	arch_arm_init,
//	arch_arm_done,
	NULL,
	arch_arm_get_pc,
	NULL,
	NULL,
	arch_arm_tag_instr,
	arch_arm_disasm_instr,
	arch_arm_translate_cond,
	arch_arm_translate_instr,
        arch_arm_translate_loop_helper,
	// idbg support
	arch_arm_get_psr,
	arch_arm_get_reg,
	NULL
};

void arm_dyncom_init(arm_core_t* core){
	cpu_t* cpu = cpu_new(0, 0, arm_arch_func);
	cpu->cpu_data = (conf_object_t*)core;
	core->dyncom_cpu = get_conf_obj_by_cast(cpu, "cpu_t");
	return;
}

static void
debug_function(cpu_t *cpu) {
	return;
}

void arm_dyncom_run(cpu_t* cpu){
	arm_core_t* core = (arm_core_t*)cpu->cpu_data;
	addr_t phys_pc = core->pc;
#if 0
	if(mmu_read_(core, core->pc, PPC_MMU_CODE, &phys_pc) != PPC_MMU_OK){
		/* we donot allow mmu exception in tagging state */
		fprintf(stderr, "In %s, can not translate the pc 0x%x\n", __FUNCTION__, core->pc);
		exit(-1);
	}
#endif

	cpu->dyncom_engine->code_start = phys_pc;
        cpu->dyncom_engine->code_end = get_end_of_page(phys_pc);
        cpu->dyncom_engine->code_entry = phys_pc;

	int rc = cpu_run(cpu, debug_function);
	switch (rc) {
                case JIT_RETURN_NOERR: /* JIT code wants us to end execution */
                        break;
                case JIT_RETURN_SINGLESTEP:
                case JIT_RETURN_FUNCNOTFOUND:
                        cpu_tag(cpu, core->pc);
                        cpu->dyncom_engine->functions = 0;
                        cpu_translate(cpu);
		 /*
                  *If singlestep,we run it here,otherwise,break.
                  */
                        if (cpu->dyncom_engine->flags_debug & CPU_DEBUG_SINGLESTEP){
                                rc = cpu_run(cpu, debug_function);
                                if(rc != JIT_RETURN_TRAP)
                                        break;
                        }
                        else
                                break;
		case JIT_RETURN_TRAP:
			break;
		default:
                        fprintf(stderr, "unknown return code: %d\n", rc);
			skyeye_exit(-1);
        }// switch (rc)
	return;
}
/**
* @brief Debug function that will be called in every instruction execution, print the cpu state
*
* @param cpu the cpu_t instance
*/

void arm_dyncom_stop(){
}

void arm_dyncom_fini(){
}

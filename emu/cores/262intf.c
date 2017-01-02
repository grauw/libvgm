#include <stdlib.h>

#include <stdtype.h>
#include "../EmuStructs.h"
#include "../EmuCores.h"
#include "../EmuHelper.h"

#include "262intf.h"
#ifdef EC_YMF262_MAME
#include "ymf262.h"
#endif
#ifdef EC_YMF262_ADLIBEMU
#define OPLTYPE_IS_OPL3
#include "adlibemu.h"
#endif


static UINT8 device_start_ymf262_mame(const DEV_GEN_CFG* cfg, DEV_INFO* retDevInf);
static UINT8 device_start_ymf262_adlibemu(const DEV_GEN_CFG* cfg, DEV_INFO* retDevInf);



#ifdef EC_YMF262_MAME
static DEVDEF_RWFUNC devFunc262_MAME[] =
{
	{RWF_REGISTER | RWF_WRITE, DEVRW_A8D8, 0, ymf262_write},
	{RWF_REGISTER | RWF_READ, DEVRW_A8D8, 0, ymf262_read},
	{0x00, 0x00, 0, NULL}
};
static DEV_DEF devDef262_MAME =
{
	"YMF262", "MAME", FCC_MAME,
	
	device_start_ymf262_mame,
	ymf262_shutdown,
	ymf262_reset_chip,
	ymf262_update_one,
	
	NULL,	// SetOptionBits
	ymf262_set_mutemask,
	NULL,	// SetPanning
	NULL,	// SetSampleRateChangeCallback
	
	devFunc262_MAME,	// rwFuncs
};
#endif
#ifdef EC_YMF262_ADLIBEMU
static DEVDEF_RWFUNC devFunc262_Emu[] =
{
	{RWF_REGISTER | RWF_WRITE, DEVRW_A8D8, 0, adlib_OPL3_writeIO},
	{RWF_REGISTER | RWF_READ, DEVRW_A8D8, 0, adlib_OPL3_reg_read},
	{0x00, 0x00, 0, NULL}
};
static DEV_DEF devDef262_AdLibEmu =
{
	"YMF262", "AdLibEmu", FCC_ADLE,
	
	device_start_ymf262_adlibemu,
	adlib_OPL3_stop,
	adlib_OPL3_reset,
	adlib_OPL3_getsample,
	
	NULL,	// SetOptionBits
	adlib_OPL3_set_mute_mask,
	NULL,	// SetPanning
	NULL,	// SetSampleRateChangeCallback
	
	devFunc262_Emu,	// rwFuncs
};
#endif

const DEV_DEF* devDefList_YMF262[] =
{
#ifdef EC_YMF262_ADLIBEMU
	&devDef262_AdLibEmu,	// default, because it's better than MAME
#endif
#ifdef EC_YMF262_MAME
	&devDef262_MAME,
#endif
	NULL
};


#ifdef EC_YMF262_MAME
static UINT8 device_start_ymf262_mame(const DEV_GEN_CFG* cfg, DEV_INFO* retDevInf)
{
	void* chip;
	DEV_DATA* devData;
	UINT32 clock;
	UINT32 rate;
	
	clock = CHPCLK_CLOCK(cfg->clock);
	
	rate = clock / 288;
	SRATE_CUSTOM_HIGHEST(cfg->srMode, rate, cfg->smplRate);
	
	chip = ymf262_init(clock, rate);
	if (chip == NULL)
		return 0xFF;
	
	// YMF262 setup
	//ymf262_set_timer_handler (chip, TimerHandler, chip);
	//ymf262_set_irq_handler   (chip, IRQHandler, chip);
	//ymf262_set_update_handler(chip, stream_update262_mame, chip);
	
	devData = (DEV_DATA*)chip;
	devData->chipInf = chip;
	retDevInf->dataPtr = devData;
	retDevInf->sampleRate = rate;
	retDevInf->devDef = &devDef262_MAME;
	return 0x00;
}
#endif

#ifdef EC_YMF262_ADLIBEMU
static UINT8 device_start_ymf262_adlibemu(const DEV_GEN_CFG* cfg, DEV_INFO* retDevInf)
{
	void* chip;
	DEV_DATA* devData;
	UINT32 clock;
	UINT32 rate;
	
	clock = CHPCLK_CLOCK(cfg->clock);
	
	rate = clock / 288;
	SRATE_CUSTOM_HIGHEST(cfg->srMode, rate, cfg->smplRate);
	
	chip = adlib_OPL3_init(clock, rate);
	if (chip == NULL)
		return 0xFF;
	
	devData = (DEV_DATA*)chip;
	devData->chipInf = chip;
	retDevInf->dataPtr = devData;
	retDevInf->sampleRate = rate;
	retDevInf->devDef = &devDef262_AdLibEmu;
	return 0x00;
}
#endif
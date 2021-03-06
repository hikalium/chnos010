
#include "core.h"

uint Sheet32_Internal_SetBuffer(UI_Sheet *sheet, void *vram, uint xsize, uint ysize, uint bpp)
{
	if(xsize > SHEET_MAX_XSIZE){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Internal_SetBuffer:Too large xsize.\n");
		#endif
		return 1;
	}
	if(ysize > SHEET_MAX_YSIZE){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Internal_SetBuffer:Too large ysize.\n");
		#endif
		return 2;
	}

	sheet->size.x = xsize;
	sheet->size.y = ysize;
	sheet->bpp = bpp;

	if(sheet->flags.bit.vram_auto_allocated){
		System_Memory_Free(sheet->vram, sheet->vramsize);
		sheet->flags.bit.vram_auto_allocated = False;
	}

	if(vram == Null){
		sheet->vram = System_Memory_Allocate(xsize * ysize * 4);
		sheet->flags.bit.vram_auto_allocated = True;
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Internal_SetBuffer:Auto Allocation.\n");
		#endif
	} else{
		sheet->vram = vram;
	}

	sheet->vramsize = xsize * ysize * 4;
	sheet->Config_Functions = &Sheet32_Config_Functions;
	sheet->IsVisiblePixel = &Sheet_Internal_IsVisiblePixel_Invalid;
	sheet->flags.bit.using_invcol = False;

	sheet->Drawing.Fill_Rectangle = &Sheet32_Drawing_Fill_Rectangle;
	sheet->Drawing.Put_String = &Sheet32_Drawing_Put_String;
	sheet->Drawing.Draw_Point = &Sheet32_Drawing_Draw_Point;

	sheet->flags.bit.buffer_configured = True;

	return 0;
}

//set function address from parent's bpp and own bpp.
uint Sheet32_Config_Functions(UI_Sheet *sheet)
{
	if(sheet == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Config_Functions:Null sheet.\n");
		#endif
		return 0;
	}
	if(sheet->parent == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Config_Functions:Null parent.\n");
		#endif
		return 1;
	}

	if(sheet->parent->bpp == 32){
		sheet->RefreshSheet = &Sheet32_Internal_RefreshSheet_To_32;
	} else if(sheet->parent->bpp == 16){
		sheet->RefreshSheet = &Sheet32_Internal_RefreshSheet_To_16;
	} else if(sheet->parent->bpp == 8){
		sheet->RefreshSheet = &Sheet32_Internal_RefreshSheet_To_08_xy;
	} else{
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Config_Functions:Not implemented refresh function to bpp %d.\n", sheet->parent->bpp);
		#endif
	}
	return 0;
}

uint Sheet32_Internal_RefreshSheet_To_32(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	DATA_Location2D p, r;
	int x, y;

	if(sheet->parent->map == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Internal_RefreshSheet_To_32:Null map.\n");
		#endif
		return 1;
	}

	Sheet_Internal_GetLocationP(sheet, &p);
	Sheet_Internal_GetLocationR(sheet, &r);

	if(p.x < px0){
		p.x = px0;
	}
	if(p.y < py0){
		p.y = py0;
	}
	if(r.x > px1){
		r.x = px1;
	}
	if(r.y > py1){
		r.y = py1;
	}

	if(p.x < 0){
		p.x = 0;
	}
	if(p.y < 0){
		p.y = 0;
	}
	if(r.x >= (int)sheet->parent->size.x){
		r.x = (int)sheet->parent->size.x - 1;
	}
	if(r.y >= (int)sheet->parent->size.y){
		r.y = (int)sheet->parent->size.y - 1;
	}

	for(y = p.y; y <= r.y; y++){
		for(x = p.x; x <= r.x; x++){
			if(sheet->parent->map[y * sheet->parent->size.x + x] == (uint)sheet){
				((uint *)sheet->parent->vram)[y * sheet->parent->size.x + x] = ((uint *)sheet->vram)[(y - sheet->location.y) * sheet->size.x + (x - sheet->location.x)];
			}
		}
	}

	return 0;
}

uint Sheet32_Internal_RefreshSheet_To_16(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	DATA_Location2D p, r;
	int x, y;

	if(sheet->parent->map == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Internal_RefreshSheet_To_16:Null map.\n");
		#endif
		return 1;
	}

	Sheet_Internal_GetLocationP(sheet, &p);
	Sheet_Internal_GetLocationR(sheet, &r);

	if(p.x < px0){
		p.x = px0;
	}
	if(p.y < py0){
		p.y = py0;
	}
	if(r.x > px1){
		r.x = px1;
	}
	if(r.y > py1){
		r.y = py1;
	}

	if(p.x < 0){
		p.x = 0;
	}
	if(p.y < 0){
		p.y = 0;
	}
	if(r.x >= (int)sheet->parent->size.x){
		r.x = (int)sheet->parent->size.x - 1;
	}
	if(r.y >= (int)sheet->parent->size.y){
		r.y = (int)sheet->parent->size.y - 1;
	}

	for(y = p.y; y <= r.y; y++){
		for(x = p.x; x <= r.x; x++){
			if(sheet->parent->map[y * sheet->parent->size.x + x] == (uint)sheet){
				((ushort *)sheet->parent->vram)[y * sheet->parent->size.x + x] = RGB_32_To_16(((uint *)sheet->vram)[(y - sheet->location.y) * sheet->size.x + (x - sheet->location.x)]);
			}
		}
	}

	return 0;
}


uint Sheet32_Internal_RefreshSheet_To_08_xy(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	DATA_Location2D p, r;
	int x, y;

	if(sheet->parent->map == Null){
		#ifdef CHNOSPROJECT_DEBUG_SHEET
			debug("Sheet32_Internal_RefreshSheet_To_08_xy:Null map.\n");
		#endif
		return 1;
	}

	Sheet_Internal_GetLocationP(sheet, &p);
	Sheet_Internal_GetLocationR(sheet, &r);

	if(p.x < px0){
		p.x = px0;
	}
	if(p.y < py0){
		p.y = py0;
	}
	if(r.x > px1){
		r.x = px1;
	}
	if(r.y > py1){
		r.y = py1;
	}

	if(p.x < 0){
		p.x = 0;
	}
	if(p.y < 0){
		p.y = 0;
	}
	if(r.x >= (int)sheet->parent->size.x){
		r.x = (int)sheet->parent->size.x - 1;
	}
	if(r.y >= (int)sheet->parent->size.y){
		r.y = (int)sheet->parent->size.y - 1;
	}

	for(y = p.y; y <= r.y; y++){
		for(x = p.x; x <= r.x; x++){
			if(sheet->parent->map[y * sheet->parent->size.x + x] == (uint)sheet){
				((uchar *)sheet->parent->vram)[y * sheet->parent->size.x + x] = RGB_32_To_08_xy(((uint *)sheet->vram)[(y - sheet->location.y) * sheet->size.x + (x - sheet->location.x)], x, y);
			}
		}
	}

	return 0;
}

bool Sheet32_Internal_IsVisiblePixel(UI_Sheet *sheet, int px, int py)
{
	if(((uint *)(sheet->vram))[(py - sheet->location.y) * (int)sheet->size.x + (px - sheet->location.x)] >> 24 == 0xff){
		return False;
	}

	if(((uint *)(sheet->vram))[(py - sheet->location.y) * (int)sheet->size.x + (px - sheet->location.x)] == sheet->invcol){
		return False;
	}

	return True;
}

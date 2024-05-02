#ifdef __GX__
#include <gccore.h>
#endif // __GX__

#include "glN64.h"
#include "Debug.h"
#include "F3D.h"
#include "F3DEX.h"
#include "F3DEX2.h"
#include "F3DCBFD.h"
#include "S2DEX.h"
#include "S2DEX2.h"
#include "N64.h"
#include "RSP.h"
#include "RDP.h"
#include "gSP.h"
#include "gDP.h"
#include "GBI.h"

void F3DCBFD_MoveMem( u32 w0, u32 w1 )
{
	u32 offset = _SHIFTR( w0, 8, 8 ) << 3;

	switch (_SHIFTR( w0, 0, 8 ))
	{
		case F3DCBFD_MV_NORMAL:
			gSPSetVertexColorBase( w1 );
			break;
		case G_MV_LIGHT:
			if (offset >= 96)
			{
				gSPLight( w1, (offset - 48) / 48 );
			}
			break;
		default:
			F3DEX2_MoveMem( w0, w1 );
			break;
	}
}

void F3DCBFD_Vtx( u32 w0, u32 w1 )
{
	u32 n = _SHIFTR( w0, 12, 8 );

	gSPNIVertex( w1, n, _SHIFTR( w0, 1, 7 ) - n );
}

void F3DCBFD_MoveWord( u32 w0, u32 w1 )
{
	switch (_SHIFTR( w0, 16, 8 ))
	{
		case G_MW_NUMLIGHT:
			gSPNumLights( w1 / 48 );
			break;
		default:
			F3DEX2_MoveWord( w0, w1 );
			break;
	}
}

void F3DCBFD_Tri4( u32 w0, u32 w1 )
{
	gSP4Triangles( _SHIFTR( w0, 23, 5 ), _SHIFTR( w0, 18, 5 ), (_SHIFTR( w0, 15, 3 ) << 2) | (w1 >> 30),
	               _SHIFTR( w0, 10, 5 ), _SHIFTR( w0,  5, 5 ),  _SHIFTR( w0,  0, 5 ),
	               _SHIFTR( w1, 25, 5 ), _SHIFTR( w1, 20, 5 ),  _SHIFTR( w1, 15, 5 ),
	               _SHIFTR( w1, 10, 5 ), _SHIFTR( w1,  5, 5 ),  _SHIFTR( w1,  0, 5 ) );
}

void F3DCBFD_Init()
{
	// Set GeometryMode flags
	GBI_InitFlags(F3DEX2);

	GBI.PCStackSize = 10;

	//          GBI Command				Command Value				Command Function
	GBI_SetGBI( G_RDPHALF_2,			F3DEX2_RDPHALF_2,			F3D_RDPHalf_2 );
	GBI_SetGBI( G_SETOTHERMODE_H,		F3DEX2_SETOTHERMODE_H,		F3DEX2_SetOtherMode_H );
	GBI_SetGBI( G_SETOTHERMODE_L,		F3DEX2_SETOTHERMODE_L,		F3DEX2_SetOtherMode_L );
	GBI_SetGBI( G_RDPHALF_1,			F3DEX2_RDPHALF_1,			F3D_RDPHalf_1 );
	GBI_SetGBI( G_SPNOOP,				F3DEX2_SPNOOP,				F3D_SPNoOp );
	GBI_SetGBI( G_ENDDL,				F3DEX2_ENDDL,				F3D_EndDL );
	GBI_SetGBI( G_DL,					F3DEX2_DL,					F3D_DList );
	GBI_SetGBI( G_LOAD_UCODE,			F3DEX2_LOAD_UCODE,			F3DEX_Load_uCode );
	GBI_SetGBI( G_MOVEMEM,				F3DEX2_MOVEMEM,				F3DCBFD_MoveMem );
	GBI_SetGBI( G_MOVEWORD,				F3DEX2_MOVEWORD,			F3DCBFD_MoveWord );
	GBI_SetGBI( G_MTX,					F3DEX2_MTX,					F3DEX2_Mtx );
	GBI_SetGBI( G_GEOMETRYMODE,			F3DEX2_GEOMETRYMODE,		F3DEX2_GeometryMode );
	GBI_SetGBI( G_POPMTX,				F3DEX2_POPMTX,				F3DEX2_PopMtx );
	GBI_SetGBI( G_TEXTURE,				F3DEX2_TEXTURE,				F3DEX2_Texture );
	GBI_SetGBI( G_DMA_IO,				F3DEX2_DMA_IO,				F3DEX2_DMAIO );
	GBI_SetGBI( G_SPECIAL_1,			F3DEX2_SPECIAL_1,			F3DEX2_Special_1 );
	GBI_SetGBI( G_SPECIAL_2,			F3DEX2_SPECIAL_2,			F3DEX2_Special_2 );
	GBI_SetGBI( G_SPECIAL_3,			F3DEX2_SPECIAL_3,			F3DEX2_Special_3 );

	GBI_SetGBI( G_VTX,					F3DEX2_VTX,					F3DCBFD_Vtx );
	GBI_SetGBI( G_MODIFYVTX,			F3DEX2_MODIFYVTX,			F3DEX_ModifyVtx );
	GBI_SetGBI( G_CULLDL,				F3DEX2_CULLDL,				F3DEX_CullDL );
	GBI_SetGBI( G_BRANCH_Z,				F3DEX2_BRANCH_Z,			F3DEX_Branch_Z );
	GBI_SetGBI( G_TRI1,					F3DEX2_TRI1,				F3DEX2_Tri1 );
	GBI_SetGBI( G_TRI2,					F3DEX2_TRI2,				F3DEX_Tri2 );
	GBI_SetGBI( G_QUAD,					F3DEX2_QUAD,				F3DEX2_Quad );
//	GBI_SetGBI( G_LINE3D,				F3DEX2_LINE3D,				F3DEX2_Line3D );

	for (int i = 0x10; i <= 0x1F; i++)
		GBI.cmd[i] = F3DCBFD_Tri4;

	GBI_SetGBI( G_BG_1CYC,				S2DEX2_BG_1CYC,				S2DEX_BG_1Cyc );
	GBI_SetGBI( G_BG_COPY,				S2DEX2_BG_COPY,				S2DEX_BG_Copy );
	GBI_SetGBI( G_OBJ_RENDERMODE,		S2DEX2_OBJ_RENDERMODE,		S2DEX_Obj_RenderMode );
}


// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
//
// NightFox LIB - Funciones 2D comunes
// http://www.nightfoxandco.com/

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_sprite256.h"
#include "nf_tiledbg.h"

void NF_Set2D(u8 screen, u8 mode)
{
    if (screen == 0)
    {
        // Top screen
        switch (mode)
        {
            case 0:
                videoSetMode(MODE_0_2D);
                break;
            case 2:
                videoSetMode(MODE_2_2D);
                break;
            case 5:
                videoSetMode(MODE_5_2D);
                break;
        }
    }
    else
    {
        // Bottom screen
        switch (mode)
        {
            case 0:
                videoSetModeSub(MODE_0_2D);
                break;
            case 2:
                videoSetModeSub(MODE_2_2D);
                break;
            case 5:
                videoSetModeSub(MODE_5_2D);
                break;
        }
    }
}

void NF_ShowBg(u8 screen, u8 layer)
{
    if (screen == 0)
    {
        // Top screen
        switch (layer)
        {
            case 0:
                REG_DISPCNT |= DISPLAY_BG0_ACTIVE;
                break;
            case 1:
                REG_DISPCNT |= DISPLAY_BG1_ACTIVE;
                break;
            case 2:
                REG_DISPCNT |= DISPLAY_BG2_ACTIVE;
                break;
            case 3:
                REG_DISPCNT |= DISPLAY_BG3_ACTIVE;
                break;
        }
    }
    else
    {
        // Bottom screen
        switch (layer)
        {
            case 0:
                REG_DISPCNT_SUB |= DISPLAY_BG0_ACTIVE;
                break;
            case 1:
                REG_DISPCNT_SUB |= DISPLAY_BG1_ACTIVE;
                break;
            case 2:
                REG_DISPCNT_SUB |= DISPLAY_BG2_ACTIVE;
                break;
            case 3:
                REG_DISPCNT_SUB |= DISPLAY_BG3_ACTIVE;
                break;
        }
    }
}

void NF_HideBg(u8 screen, u8 layer)
{
    if (screen == 0)
    {
        // Top screen
        switch (layer)
        {
            case 0:
                REG_DISPCNT &= ~DISPLAY_BG0_ACTIVE;
                break;
            case 1:
                REG_DISPCNT &= ~DISPLAY_BG1_ACTIVE;
                break;
            case 2:
                REG_DISPCNT &= ~DISPLAY_BG2_ACTIVE;
                break;
            case 3:
                REG_DISPCNT &= ~DISPLAY_BG3_ACTIVE;
                break;
        }
    }
    else
    {
        // Bottom screen
        switch (layer)
        {
            case 0:
                REG_DISPCNT_SUB &= ~DISPLAY_BG0_ACTIVE;
                break;
            case 1:
                REG_DISPCNT_SUB &= ~DISPLAY_BG1_ACTIVE;
                break;
            case 2:
                REG_DISPCNT_SUB &= ~DISPLAY_BG2_ACTIVE;
                break;
            case 3:
                REG_DISPCNT_SUB &= ~DISPLAY_BG3_ACTIVE;
                break;
        }
    }
}

void NF_ScrollBg(u8 screen, u8 layer, s16 x, s16 y)
{
    // Temporary variables
    s16 sx = x;
    s16 sy = y;

    // If the map is infinite (> 512 tiles)
    if (NF_TILEDBG_LAYERS[screen][layer].bgtype > 0)
    {
        // Temporary variables for infinite backgrounds
        u32 address = 0;    // VRAM address
        u16 blockx = 0;     // Number of block in the screen
        u16 blocky = 0;
        u32 mapmovex = 0;   // Data copy offset (block x 2048)
        u32 mapmovey = 0;
        u16 rowsize = 0;    // Size used by each row in RAM

        // Calculate base address of the map in VRAM
        if (screen == 0) // VRAM_A
            address = 0x6000000 + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11);
        else // VRAM_C
            address = 0x6200000 + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11);

        // Limit scroll variables
        if (sx < 0)
            sx = 0;
        if (sx > (NF_TILEDBG_LAYERS[screen][layer].bgwidth - 256))
            sx = NF_TILEDBG_LAYERS[screen][layer].bgwidth - 256;

        if (sy < 0)
            sy = 0;
        if (sy > (NF_TILEDBG_LAYERS[screen][layer].bgheight - 192))
            sy = NF_TILEDBG_LAYERS[screen][layer].bgheight - 192;

        // Handle the different types of map
        switch (NF_TILEDBG_LAYERS[screen][layer].bgtype)
        {
            // 512x256 - Block A and B (32x32) + (32x32) (2kb x 2 = 4kb)
            case 1:
                // Calculate block
                blockx = x >> 8;

                // If you have changed block...
                if (NF_TILEDBG_LAYERS[screen][layer].blockx != blockx)
                {
                    // Calculate data offset
                    mapmovex = blockx << 11;

                    // Copy blocks A and B (32x32) + (32x32) (2kb x 2 = 4kb)
                    NF_DmaMemCopy((void *)address,
                        NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mapmovex,
                        4096);

                    // Update the current block
                    NF_TILEDBG_LAYERS[screen][layer].blockx = blockx;
                }

                // Calculate horizontal scroll
                sx = x - (blockx << 8);
                break;

            // 256x512 - Block A (32x64) (2kb x 2 = 4kb)
            case 2:
                // Calculate block
                blocky = y >> 8;

                // If you have changed block...
                if (NF_TILEDBG_LAYERS[screen][layer].blocky != blocky)
                {
                    // Calculate data offset
                    mapmovey = blocky << 11;

                    // Copy blocks A and B (32x32) + (32x32) (2kb x 2 = 4kb)
                    NF_DmaMemCopy((void *)address,
                        NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mapmovey,
                        4096);

                    // Update the current block
                    NF_TILEDBG_LAYERS[screen][layer].blocky = blocky;
                }

                // Calculate vertical scroll
                sy = y - (blocky << 8);
                break;

            // >512 x >512
            case 3:
                rowsize = (((NF_TILEDBG_LAYERS[screen][layer].bgwidth - 1) >> 8) + 1) << 11;

                // Calculate blocks
                blockx = x >> 8;
                blocky = y >> 8;

                // If you have changed block in any direction...
                if ((NF_TILEDBG_LAYERS[screen][layer].blockx != blockx)
                 || (NF_TILEDBG_LAYERS[screen][layer].blocky != blocky))
                {
                    // Calculate data offset
                    mapmovex = (blocky * rowsize) + (blockx << 11);
                    mapmovey = mapmovex + rowsize;

                    // Blocks A and B (32x32) + (32x32) (2kb x 2 = 4kb)
                    NF_DmaMemCopy((void *)address,
                        NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mapmovex,
                        4096);

                    // Blocks (+4096) C and D (32x32) + (32x32) (2kb x 2 = 4kb)
                    NF_DmaMemCopy((void *)(address + 4096),
                        NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mapmovey,
                        4096);

                    // Update the current block
                    NF_TILEDBG_LAYERS[screen][layer].blockx = blockx;
                    NF_TILEDBG_LAYERS[screen][layer].blocky = blocky;
                }

                // Calculate horizontal and vertical scrolls
                sx = x - (blockx << 8);
                sy = y - (blocky << 8);
                break;
        }
    }

    // Set the scroll in the hardware registers
    if (screen == 0)
    {
        // Top screen
        switch (layer)
        {
            case 0:
                REG_BG0HOFS = sx;
                REG_BG0VOFS = sy;
                break;
            case 1:
                REG_BG1HOFS = sx;
                REG_BG1VOFS = sy;
                break;
            case 2:
                REG_BG2HOFS = sx;
                REG_BG2VOFS = sy;
                break;
            case 3:
                REG_BG3HOFS = sx;
                REG_BG3VOFS = sy;
                break;
        }
    }
    else
    {
        // Bottom screen
        switch (layer)
        {
            case 0:
                REG_BG0HOFS_SUB = sx;
                REG_BG0VOFS_SUB = sy;
                break;
            case 1:
                REG_BG1HOFS_SUB = sx;
                REG_BG1VOFS_SUB = sy;
                break;
            case 2:
                REG_BG2HOFS_SUB = sx;
                REG_BG2VOFS_SUB = sy;
                break;
            case 3:
                REG_BG3HOFS_SUB = sx;
                REG_BG3VOFS_SUB = sy;
                break;
        }
    }
}

void NF_SpriteFrame(u8 screen, u8 id, u16 frame)
{
    // Verify that the sprite ID is valid
    if (id > 127)
        NF_Error(106, "Sprite", 127);

    // Verify that the frame index exists
    if (frame > NF_SPRITEOAM[screen][id].lastframe)
        NF_Error(106, "Sprite frame", NF_SPRITEOAM[screen][id].lastframe);

    // Check if the frame needs to be updated
    if (NF_SPRITEOAM[screen][id].frame != frame)
    {
        if (NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][id].gfxid].keepframes)
        {
            // If we need to automatically copy frames from RAM to VRAM because
            // they haven't been copied to VRAM when loading the sprite.

            // Temporary variables
            char *source;           // Source pointer
            u32 destination = 0;    // Destination pointer
            u16 ramid = 0;          // RAM slot that contains the graphics of the frame

            // Calculate source and destination for the copy
            ramid =  NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][id].gfxid].ramid;
            source = NF_BUFFER_SPR256GFX[ramid] + (NF_SPRITEOAM[screen][id].framesize * frame);
            destination = NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][id].gfxid].address;

            NF_DmaMemCopy((void *)destination, source, NF_SPRITEOAM[screen][id].framesize);
        }
        else
        {
            // If all frames are in VRAM

            // Calculate the address of the graphics of the frame
            u32 address = 0;
            address = NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][id].gfxid].address
                    + (NF_SPRITEOAM[screen][id].framesize * frame);
            NF_SPRITEOAM[screen][id].gfx = (u32*)address;

        }

        // Set the current frame
        NF_SPRITEOAM[screen][id].frame = frame;
    }
}

void NF_EnableSpriteRotScale(u8 screen, u8 sprite, u8 id, bool doublesize)
{
    // Verify that the sprite ID is valid
    if (sprite > 127)
        NF_Error(106, "Sprite", 127);

    // Verifica el rango de Id's de Rotacion
    if (id > 31) {
        NF_Error(106, "RotScale", 127);
    }

    // Verify that the sprite has been created
    if (!NF_SPRITEOAM[screen][sprite].created)
    {
        char text[4];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(112, text, sprite);
    }

    // Rotation ID (-1 means none). Valid IDs are 0 to 31
    NF_SPRITEOAM[screen][sprite].rot = id;
    // Enable or disable "double size" mode when rotating
    NF_SPRITEOAM[screen][sprite].doublesize = doublesize;
}

void NF_DisableSpriteRotScale(u8 screen, u8 sprite)
{
    // Verify that the sprite ID is valid
    if (sprite > 127)
        NF_Error(106, "Sprite", 127);

    // Verify that the sprite has been created
    if (!NF_SPRITEOAM[screen][sprite].created)
    {
        char text[4];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(112, text, sprite);
    }

    // Rotation ID (-1 means none). Valid IDs are 0 to 31
    NF_SPRITEOAM[screen][sprite].rot = -1;
    // Disable "double size" mode when rotating
    NF_SPRITEOAM[screen][sprite].doublesize = false;
}

void NF_SpriteRotScale(u8 screen, u8 id, s16 angle, u16 sx, u16 sy)
{
    // Temporary variables
    s16 in = 0;     // Input angle
    s16 out = 0;    // Converted angle

    in = angle;

    // Angle limits
    if (in < -512)
        in += 512;
    if (in > 512)
        in -= 512;

    // Scale X limits
    if (sx > 512)
        sx = 512;

    // Scale Y limits
    if (sy > 512)
        sy = 512;

    if (in < 0) // If the angle is negative
    {
        in = -in; // Make it positive (to be able to do a bitshift)
        out = in << 6; // (in * 64); Switch from base 512 to base 32768
        // Leave it positive so that <0 rotates counter-clockwise
    }
    else
    {
        out = in << 6;
        out = -out; // Make it negative so that <0 rotates clockwise
    }

    // Update rotation and scale in OAM
    if (screen == 0)
        oamRotateScale(&oamMain, id, out, 512 - sx, 512 - sy);
    else
        oamRotateScale(&oamSub, id, out, 512 - sx, 512 - sy);
}

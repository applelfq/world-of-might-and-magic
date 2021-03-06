#include "Engine/AssetsManager.h"
#include "Engine/Engine.h"
#include "Engine/Events.h"
#include "Engine/LOD.h"
#include "Engine/Localization.h"
#include "Engine/OurMath.h"
#include "Engine/Party.h"
#include "Engine/Time.h"

#include "Engine/Graphics/IRender.h"
#include "Engine/Graphics/Indoor.h"
#include "Engine/Graphics/Level/Decoration.h"
#include "Engine/Graphics/Outdoor.h"
#include "Engine/Graphics/Viewport.h"

#include "IO/Mouse.h"

#include "GUI/GUIButton.h"
#include "GUI/GUIFont.h"
#include "GUI/UI/Books/MapBook.h"
#include "GUI/UI/UIGame.h"

#include "Media/Audio/AudioPlayer.h"

void DrawBook_Map_sub(unsigned int tl_x, unsigned int tl_y, unsigned int br_x,
                      int br_y, int _48074);  // idb

Image *ui_book_map_background = nullptr;

GUIWindow_MapBook::GUIWindow_MapBook() : GUIWindow_Book() {
    this->ptr_1C =
        (void *)WINDOW_MapsBook;  // inherited from GUIWindow::GUIWindow
    BasicBookInitialization();

    pEventTimer->Pause();
    viewparams->sViewCenterX = pParty->vPosition.x;
    viewparams->sViewCenterY = pParty->vPosition.y;
    pAudioPlayer->StopChannels(-1, -1);
    pBooksButtonOverlay =
        new GUIWindow_BooksButtonOverlay(546, 353, 0, 0, (int)pBtn_Maps);

    dword_506364 = 1;
    ui_book_map_background = assets->GetImage_ColorKey("sbmap", 0x7FF);

    ui_book_button1_on = assets->GetImage_Alpha("zoom-on");
    ui_book_button2_on = assets->GetImage_Alpha("zoot-on");
    ui_book_button3_on = assets->GetImage_Alpha("tabNon");
    ui_book_button4_on = assets->GetImage_Alpha("tabSon");
    ui_book_button5_on = assets->GetImage_Alpha("tabEon");
    ui_book_button6_on = assets->GetImage_Alpha("tabWon");
    ui_book_button1_off = assets->GetImage_Alpha("zoom-off");
    ui_book_button2_off = assets->GetImage_Alpha("zoot-off");
    ui_book_button3_off = assets->GetImage_Alpha("tabNoff");
    ui_book_button4_off = assets->GetImage_Alpha("tabSoff");
    ui_book_button5_off = assets->GetImage_Alpha("tabEoff");
    ui_book_button6_off = assets->GetImage_Alpha("tabWoff");

    pBtn_Book_1 = this->CreateButton(
        pViewport->uViewportTL_X + 398, pViewport->uViewportTL_Y + 1, 50, 34, 1,
        0, UIMSG_ClickBooksBtn, 0, 0, localization->GetString(251),
        {{ui_book_button1_on}});  // "Zoom In"
    pBtn_Book_2 = this->CreateButton(
        pViewport->uViewportTL_X + 398, pViewport->uViewportTL_Y + 38, 50, 34,
        1, 0, UIMSG_ClickBooksBtn, 1, 0, localization->GetString(252),
        {{ui_book_button2_on}});  // "Zoom Out"
    pBtn_Book_3 = this->CreateButton(
        pViewport->uViewportTL_X + 397, pViewport->uViewportTL_Y + 113, 50, 34,
        1, 0, UIMSG_ClickBooksBtn, 2, 0,
        localization->GetString(192));  // Scroll Up
    pBtn_Book_4 = this->CreateButton(
        pViewport->uViewportTL_X + 397, pViewport->uViewportTL_Y + 150, 50, 34,
        1, 0, UIMSG_ClickBooksBtn, 3, 0,
        localization->GetString(193));  // Scroll Down
    pBtn_Book_5 = this->CreateButton(
        pViewport->uViewportTL_X + 397, pViewport->uViewportTL_Y + 188, 50, 34,
        1, 0, UIMSG_ClickBooksBtn, 4, 0,
        localization->GetString(573));  // "Scroll Right"
    pBtn_Book_6 = this->CreateButton(
        pViewport->uViewportTL_X + 397, pViewport->uViewportTL_Y + 226, 50, 34,
        1, 0, UIMSG_ClickBooksBtn, 5, 0,
        localization->GetString(572));  // "Scroll Left"
}

void GUIWindow_MapBook::Update() {
    unsigned int map_id;  // eax@35
    // char party_coord[120]; // [sp+Ch] [bp-CCh]@37
    GUIWindow map_window;  // [sp+84h] [bp-54h]@35

    render->DrawTextureAlphaNew(471 / 640.0f, 445 / 480.0f,
        ui_exit_cancel_button_background);
    render->DrawTextureAlphaNew(pViewport->uViewportTL_X / 640.0f,
        pViewport->uViewportTL_Y / 480.0f,
        ui_book_map_background);
    if (BtnUp_flag || viewparams->uMapBookMapZoom / 128 >= 12)  // Button 1
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 408) / 640.0f,
        (pViewport->uViewportTL_Y + 2) / 480.0f,
            ui_book_button1_off);
    else
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 398) / 640.0f,
        (pViewport->uViewportTL_Y + 1) / 480.0f,
            ui_book_button1_on);

    if (BtnDown_flag || viewparams->uMapBookMapZoom / 128 <= 3)  // Button 2
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 408) / 640.0f,
        (pViewport->uViewportTL_Y + 38) / 480.0f,
            ui_book_button2_off);
    else
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 398) / 640.0f,
        (pViewport->uViewportTL_Y + 38) / 480.0f,
            ui_book_button2_on);

    if (Book_PageBtn3_flag)  // Button 3
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 408) / 640.0f,
        (pViewport->uViewportTL_Y + 113) / 480.0f,
            ui_book_button3_off);
    else
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 398) / 640.0f,
        (pViewport->uViewportTL_Y + 113) / 480.0f,
            ui_book_button3_on);

    if (Book_PageBtn4_flag)  // Button 4
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 408) / 640.0f,
        (pViewport->uViewportTL_Y + 150) / 480.0f,
            ui_book_button4_off);
    else
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 399) / 640.0f,
        (pViewport->uViewportTL_Y + 150) / 480.0f,
            ui_book_button4_on);

    if (Book_PageBtn5_flag)  // Button 5
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 408) / 640.0f,
        (pViewport->uViewportTL_Y + 188) / 480.0f,
            ui_book_button5_off);
    else
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 397) / 640.0f,
        (pViewport->uViewportTL_Y + 188) / 480.0f,
            ui_book_button5_on);

    if (Book_PageBtn6_flag)  // Button 6
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 408) / 640.0f,
        (pViewport->uViewportTL_Y + 226) / 480.0f,
            ui_book_button6_off);
    else
        render->DrawTextureAlphaNew((pViewport->uViewportTL_X + 397) / 640.0f,
        (pViewport->uViewportTL_Y + 226) / 480.0f,
            ui_book_button6_on);

    if (BtnDown_flag) viewparams->CenterOnParty2();
    if (BtnUp_flag) viewparams->CenterOnParty();
    if (Book_PageBtn3_flag) viewparams->_443219();
    if (Book_PageBtn4_flag) viewparams->_443231();
    if (Book_PageBtn5_flag) viewparams->_44323D();
    if (Book_PageBtn6_flag) viewparams->_443225();

    if (BtnUp_flag | BtnDown_flag | Book_PageBtn3_flag | Book_PageBtn4_flag |
        Book_PageBtn5_flag | Book_PageBtn6_flag)
        pAudioPlayer->PlaySound(SOUND_StartMainChoice02, 0, 0, -1, 0, 0);
    BtnUp_flag = 0;
    BtnDown_flag = 0;
    Book_PageBtn6_flag = 0;
    Book_PageBtn5_flag = 0;
    Book_PageBtn4_flag = 0;
    Book_PageBtn3_flag = 0;

    DrawBook_Map_sub(97, 49, 361, 313, 0);
    map_window.uFrameWidth = game_viewport_width;
    map_window.uFrameHeight = game_viewport_height;
    map_window.uFrameX = game_viewport_x;
    map_window.uFrameY = game_viewport_y;
    map_window.uFrameZ = game_viewport_z;
    map_window.uFrameW = game_viewport_w;
    map_id = pMapStats->GetMapInfo(pCurrentMapName);

    if (map_id)
        map_window.DrawTitleText(pBook2Font, -14, 12, ui_book_map_title_color,
            pMapStats->pInfos[map_id].pName, 3);

    auto party_coordinates = localization->FormatString(
        659, pParty->vPosition.x, pParty->vPosition.y);  // "x: %d  y: %d"

    map_window.uFrameX = 0;
    map_window.DrawTitleText(pFontComic, 0, 320, ui_book_map_coordinates_color,
        party_coordinates, 0);
}

//----- (00442955) --------------------------------------------------------
void DrawBook_Map_sub(unsigned int tl_x, unsigned int tl_y, unsigned int br_x,
                      int br_y, int _48074) {
    int v20;                // eax@16
    int v26;                // ecx@21
    unsigned __int16 *v27;  // edi@21
    int v28;                // edx@21
    int v29;                // eax@21

    int v54;                // esi@75
    int v55;                // eax@75

    int map_tile_X;  // [sp+48020h] [bp-44h]@23

    int v87;                    // [sp+48038h] [bp-2Ch]@16
    unsigned int v88;           // [sp+4803Ch] [bp-28h]@16
    unsigned __int16 *v93;      // [sp+48050h] [bp-14h]@16
    unsigned int v95;           // [sp+48058h] [bp-Ch]@16
    int map_tile_Y;             // [sp+4805Ch] [bp-8h]@10
    const void *v97;            // [sp+48060h] [bp-4h]@16

    int ScreenCenterX = (signed int)(tl_x + br_x) / 2;
    int ScreenCenterY = (signed int)(tl_y + br_y) / 2;
    render->SetUIClipRect(tl_x, tl_y, br_x, br_y);
    int pCenterX = viewparams->sViewCenterX;
    int pCenterY = viewparams->sViewCenterY;

    if (viewparams->uMapBookMapZoom != 384) {
        if (viewparams->uMapBookMapZoom == 768) {
            if (uCurrentlyLoadedLevelType == LEVEL_Indoor)
                viewparams->uMapBookMapZoom = 680;
        }
    } else {
        viewparams->sViewCenterX = viewparams->indoor_center_x;
        pCenterX = viewparams->indoor_center_x;
        pCenterY = viewparams->indoor_center_y;
        if (uCurrentlyLoadedLevelType == LEVEL_Indoor)
            viewparams->uMapBookMapZoom = viewparams->uMapBookMapZoom - 34;
    }

    int screenWidth, screenHeight;

    if (uCurrentlyLoadedLevelType != LEVEL_Indoor) {  // outdoors
        screenWidth = br_x - tl_x + 1;
        screenHeight = br_y - tl_y + 1;

        int loc_power = ImageHelper::GetWidthLn2(viewparams->location_minimap);
        int scale_increment = (1 << (loc_power + 16)) / viewparams->uMapBookMapZoom;
        double MapSizeScale = (double)(1 << (16 - loc_power));
        int stepX_r_resets =
            (unsigned int)(signed __int64)
            ((double)(viewparams->sViewCenterX - 22528 / (viewparams->uMapBookMapZoom / 384) + 32768) / MapSizeScale) << 16;
        int stepY_r = (int)(signed __int64)
            ((double)(-pCenterY - 22528 / (viewparams->uMapBookMapZoom / 384) + 32768) / MapSizeScale) << 16;
        int scaled_posY = stepY_r >> 16;

        static Texture *minimaptemp = nullptr;
        if (!minimaptemp) {
            minimaptemp = render->CreateTexture_Blank(screenWidth, screenHeight, IMAGE_FORMAT_R8G8B8A8);
        }
        auto minitempix = (unsigned __int32 *)minimaptemp->GetPixels(IMAGE_FORMAT_R8G8B8A8);
        auto minimap_pixels = (unsigned __int32 *)viewparams->location_minimap->GetPixels(IMAGE_FORMAT_R8G8B8A8);
        int textr_width = viewparams->location_minimap->GetWidth();

        // nearest neiborhood scaling
        // if (texture8_data)
        {
            for (uint i = 0; i < screenHeight; ++i) {
                map_tile_Y = (scaled_posY - 80) / 4;
                int stepX_r = stepX_r_resets;
                for (uint j = 0; j < screenWidth; ++j) {
                    int scaled_posX = stepX_r >> 16;
                    map_tile_X = (scaled_posX - 80) / 4;
                    if (!pOutdoor->IsMapCellFullyRevealed(map_tile_X, map_tile_Y)) {
                        if (pOutdoor->IsMapCellPartiallyRevealed(map_tile_X,
                            map_tile_Y)) {
                            if (!((i + ScreenCenterX + j) % 2))
                                minitempix[j + i * screenWidth] = Color32A(12, 12, 12);  // greyed out
                            else
                                minitempix[j + i * screenWidth] = minimap_pixels[scaled_posX + scaled_posY * textr_width];
                        } else {
                            minitempix[j + i * screenWidth] = Color32A(0, 0, 0);  // black
                        }
                    } else {
                        minitempix[j + i * screenWidth] = minimap_pixels[scaled_posX + scaled_posY * textr_width];
                    }
                    stepX_r += scale_increment;
                }
                stepY_r += scale_increment;
                scaled_posY = stepY_r >> 16;
            }
        }

        render->Update_Texture(minimaptemp);
        render->DrawTextureAlphaNew(tl_x / 640., tl_y / 480., minimaptemp);

        render->ResetUIClipRect();
        render->DrawTextureAlphaNew(75 / 640.0f, 22 / 480.0f, ui_book_map_frame);
    } else {  // indoors
        unsigned int Colour_Black = Color16(0, 0, 0);
        unsigned int Colour_Teal = Color16(0, 0xFF, 0xFF);

        uNumBlueFacesInBLVMinimap = 0;

        if (pIndoor->pMapOutlines->uNumOutlines) {
            for (uint i = 0; i < pIndoor->pMapOutlines->uNumOutlines; ++i) {
                if (!(pIndoor->pFaces[pIndoor->pMapOutlines->pOutlines[i].uFace1ID].Invisible() ||
                      (pIndoor->pFaces[pIndoor->pMapOutlines->pOutlines[i].uFace2ID].Invisible()))) {
                    if (!(pIndoor->pMapOutlines->pOutlines[i].uFlags & 1)) {
                        if (!(!(pIndoor->pFaces[pIndoor->pMapOutlines->pOutlines[i].uFace1ID].uAttributes & FACE_UNKNOW7) &&
                            !(pIndoor->pFaces[pIndoor->pMapOutlines->pOutlines[i].uFace2ID].uAttributes & FACE_UNKNOW7))) {
                            pIndoor->pMapOutlines->pOutlines[i].uFlags =
                                pIndoor->pMapOutlines->pOutlines[i].uFlags | 1;

                            pIndoor->_visible_outlines[i >> 3] |= 1 << (7 - i % 8);
                        }
                    }

                    if ((!(pIndoor->pMapOutlines->pOutlines[i].uFlags & 1) &&
                         !(!(pIndoor->pFaces[pIndoor->pMapOutlines->pOutlines[i].uFace1ID].uAttributes & FACE_UNKNOW7) &&
                           !(pIndoor->pFaces[pIndoor->pMapOutlines->pOutlines[i].uFace2ID].uAttributes & FACE_UNKNOW7))) ||
                            pIndoor->pMapOutlines->pOutlines[i].uFlags & 1) {
                        int Vert1X = pIndoor->pVertices[pIndoor->pMapOutlines->pOutlines[i].uVertex1ID].x - viewparams->sViewCenterX;
                        int Vert2X = pIndoor->pVertices[pIndoor->pMapOutlines->pOutlines[i].uVertex2ID].x - viewparams->sViewCenterX;
                        int Vert1Y = pIndoor->pVertices[pIndoor->pMapOutlines->pOutlines[i].uVertex1ID].y - pCenterY;
                        int Vert2Y = pIndoor->pVertices[pIndoor->pMapOutlines->pOutlines[i].uVertex2ID].y - pCenterY;

                        int linex = ScreenCenterX + fixpoint_mul(Vert1X, viewparams->uMapBookMapZoom);
                        int liney = ScreenCenterY - fixpoint_mul(Vert1Y, viewparams->uMapBookMapZoom);
                        int linez = ScreenCenterX + fixpoint_mul(Vert2X, viewparams->uMapBookMapZoom);
                        int linew = ScreenCenterY - fixpoint_mul(Vert2Y, viewparams->uMapBookMapZoom);

                        render->RasterLine2D(linex, liney, linez, linew, Colour_Black);
                    }
                }
            }
        }

        if ((signed int)uNumBlueFacesInBLVMinimap > 0) {
            for (uint j = 0; j < (signed int)uNumBlueFacesInBLVMinimap; ++j) {
                v26 = pIndoor
                          ->pVertices
                              [pIndoor->pMapOutlines
                                   ->pOutlines[pBlueFacesInBLVMinimapIDs[j]]
                                   .uVertex2ID]
                          .x;
                v27 = (unsigned __int16
                           *)(pIndoor
                                  ->pVertices
                                      [pIndoor->pMapOutlines
                                           ->pOutlines
                                               [pBlueFacesInBLVMinimapIDs[j]]
                                           .uVertex1ID]
                                  .x -
                              pCenterX);
                v28 = pIndoor
                          ->pVertices
                              [pIndoor->pMapOutlines
                                   ->pOutlines[pBlueFacesInBLVMinimapIDs[j]]
                                   .uVertex1ID]
                          .y -
                      pCenterY;
                v29 = pIndoor
                          ->pVertices
                              [pIndoor->pMapOutlines
                                   ->pOutlines[pBlueFacesInBLVMinimapIDs[j]]
                                   .uVertex2ID]
                          .y -
                      pCenterY;

                v87 =
                    fixpoint_mul((signed int)v27, viewparams->uMapBookMapZoom);
                v88 = fixpoint_mul(v28, viewparams->uMapBookMapZoom);
                uint i =
                    fixpoint_mul((v26 - pCenterX), viewparams->uMapBookMapZoom);
                v95 = fixpoint_mul(v29, viewparams->uMapBookMapZoom);
                render->RasterLine2D(
                    ScreenCenterX +
                        (fixpoint_mul((signed int)v27,
                                      viewparams->uMapBookMapZoom)),
                    ScreenCenterY - v88,
                    ScreenCenterX +
                        (fixpoint_mul((v26 - pCenterX),
                                      viewparams->uMapBookMapZoom)),
                    ScreenCenterY - v95, Colour_Teal);
            }
            viewparams->sViewCenterX = pCenterX;
        }
    }

    // Direction arrow drawing
    int ArrowXPos = (fixpoint_mul((pParty->vPosition.x - viewparams->sViewCenterX), viewparams->uMapBookMapZoom)) + ScreenCenterX - 3;
    int ArrowYPos = ScreenCenterY - (fixpoint_mul((pParty->vPosition.y - pCenterY), viewparams->uMapBookMapZoom)) - 3;

    bool DrawArrow = 1;

    if (ArrowXPos >= (signed int)tl_x) {
        if (ArrowXPos > (signed int)br_x) {
            if ((signed int)(ArrowXPos - 3) > (signed int)br_x) DrawArrow = 0;
            ArrowXPos = br_x;
        }
    } else {
        if ((signed int)(ArrowXPos + 3) < (signed int)tl_x) DrawArrow = 0;
        ArrowXPos = tl_x;
    }

    if (ArrowYPos >= (signed int)tl_y) {
        if (ArrowYPos > br_y) {
            if ((ArrowYPos - 3) > br_y) DrawArrow = 0;
            ArrowYPos = br_y;
        }
    } else {
        if ((ArrowYPos + 3) < (signed int)tl_y) DrawArrow = 0;
        ArrowYPos = tl_y;
    }

    if (DrawArrow == 1) {
        int ArrowOctant = 0;
        int PartyDirection = pParty->sRotationY & stru_5C6E00->uDoublePiMask;
        if ((signed int)PartyDirection <= 1920) ArrowOctant = 6;
        if ((signed int)PartyDirection < 1664) ArrowOctant = 5;
        if ((signed int)PartyDirection <= 1408) ArrowOctant = 4;
        if ((signed int)PartyDirection < 1152) ArrowOctant = 3;
        if ((signed int)PartyDirection <= 896) ArrowOctant = 2;
        if ((signed int)PartyDirection < 640) ArrowOctant = 1;
        if ((signed int)PartyDirection <= 384) ArrowOctant = 0;
        if ((signed int)PartyDirection < 128 || (signed int)PartyDirection > 1920) ArrowOctant = 7;

        render->DrawTransparentRedShade(ArrowXPos / 640.0f, ArrowYPos / 480.0f, game_ui_minimap_dirs[ArrowOctant]);
    }

    if ((signed int)uNumLevelDecorations > 0) {
        for (uint i = 0; i < (signed int)uNumLevelDecorations; ++i) {
            if (pLevelDecorations[i].uFlags & LEVEL_DECORATION_VISIBLE_ON_MAP) {
                screenHeight = pLevelDecorations[i].vPosition.y - pCenterY;
                v93 = (unsigned __int16 *)(pLevelDecorations[i].vPosition.x -
                                           viewparams->sViewCenterX);
                v54 = (fixpoint_mul((signed int)v93,
                                    viewparams->uMapBookMapZoom)) +
                      ScreenCenterX;
                v97 = (const void *)(fixpoint_mul(screenHeight,
                                                  viewparams->uMapBookMapZoom));
                v55 = ScreenCenterY - (int)v97;
                // if ( v54 >= render->raster_clip_x && v54 <=
                // render->raster_clip_z
                //  && v55 >= render->raster_clip_y && v55 <=
                //  render->raster_clip_w )
                {
                    if (viewparams->uMapBookMapZoom > 512) {
                        render->RasterLine2D(v54 - 1, v55 - 1, v54 - 1, v55 + 1,
                                             Color16(0xFFu, 0xFFu, 0xFFu));
                        render->RasterLine2D(v54, v55 - 1, v54, v55 + 1,
                                             Color16(0xFFu, 0xFFu, 0xFFu));
                        render->RasterLine2D(v54 + 1, v55 - 1, v54 + 1, v55 + 1,
                                             Color16(0xFFu, 0xFFu, 0xFFu));
                    } else {
                        render->RasterLine2D(
                            v54, ScreenCenterY - (int)v97,
                            (fixpoint_mul((signed int)v93,
                                viewparams->uMapBookMapZoom)) +
                            ScreenCenterX,
                            ScreenCenterY - (int)v97,
                            Color16(0xFF, 0xFF, 0xFF));
                    }
                }
            }
        }
    }
}

const char *GetMapBookHintText(int mouse_x, int mouse_y) {
    int v20 = viewparams->sViewCenterX;
    int v21 = viewparams->sViewCenterY;
    if (viewparams->uMapBookMapZoom == 384) {
        v20 = viewparams->indoor_center_x;
        v21 = viewparams->indoor_center_y;
    }

    unsigned int pX = mouse_x;
    unsigned int pY = mouse_y;

    double v0 = 1.0 / (float)((signed int)viewparams->uMapBookMapZoom * 0.000015258789);

    int global_coord_X = (__int64)((double)(pX - 229.) * v0 + (double)v20);
    int global_coord_Y = (__int64)((double)v21 - (double)(pY - 181.) * v0);

    const char *result = "";

    // In the mapbook only lady Margaret dispays for defoult zoom(В
    // книге карты только Леди Маргарита всплывает при дефолтном зуме)
    int map_tile_X = abs(global_coord_X + 22528) / 512;
    int map_tile_Y = abs(global_coord_Y - 22528) / 512;
    if (pOutdoor->IsMapCellFullyRevealed(map_tile_X, map_tile_Y) &&
        uCurrentlyLoadedLevelType == LEVEL_Outdoor &&
        !pOutdoor->pBModels.empty()) {
        for (BSPModel &model : pOutdoor->pBModels) {
            if (int_get_vector_length(
                    abs((int)model.vBoundingCenter.x - global_coord_X),
                    abs((int)model.vBoundingCenter.y - global_coord_Y),
                    0) < model.sBoundingRadius) {
                for (ODMFace &face : model.pFaces) {
                    if (face.sCogTriggeredID) {
                        if (!(face.uAttributes & FACE_HAS_EVENT)) {
                            if (GetEventHintString(face.sCogTriggeredID)) {
                                if (_stricmp(GetEventHintString(face.sCogTriggeredID), "")) {
                                    result = GetEventHintString(face.sCogTriggeredID);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

#include "SonicMania.h"

ObjectUFO_Player *UFO_Player;

void UFO_Player_Update(void)
{
    RSDK_THIS(UFO_Player);

    StateMachine_Run(self->stateInput);
    StateMachine_Run(self->state);

    RSDK.ProcessAnimation(&self->animator);
    if (UFO_Setup->timedOut)
        self->groundVel -= self->groundVel >> 5;
}

void UFO_Player_LateUpdate(void)
{
    RSDK_THIS(UFO_Player);
    Matrix *mat = &UFO_Camera->matWorld;

    self->depth3D = mat->values[2][3] + mat->values[2][0] * (self->position.x >> 0x10) + mat->values[2][2] * (self->position.y >> 0x10)
                    + mat->values[2][1] * (self->height >> 0x10);
}

void UFO_Player_StaticUpdate(void) {}

void UFO_Player_Draw(void)
{
    RSDK_THIS(UFO_Player);
    if (self->depth3D >= 1) {
        RSDK.Prepare3DScene(UFO_Player->sceneIndex);
        int32 anim = self->animator.animationID;
        if (anim == 2 || anim == 3) {
            RSDK.MatrixTranslateXYZ(&self->matrix2, self->position.x, self->height + 0x100000, self->position.y, true);
            RSDK.MatrixRotateX(&self->matrix1, self->field_74);
            RSDK.MatrixRotateY(&self->matrix3, self->angle + (self->angleZ >> 5));
            RSDK.MatrixMultiply(&self->matrix4, &self->matrix1, &self->matrix3);
            RSDK.MatrixMultiply(&self->matrix3, &self->matrix4, &self->matrix2);
            RSDK.MatrixMultiply(&self->matrix3, &self->matrix3, &UFO_Camera->matWorld);
            RSDK.MatrixMultiply(&self->matrix4, &self->matrix4, &UFO_Camera->matView);
            RSDK.AddModelTo3DScene(self->animator.animationID, UFO_Player->sceneIndex, S3D_FLATCLR_SHADED_BLENDED_SCREEN, &self->matrix3,
                                   &self->matrix4, 0xFFFFFF);
        }
        else {
            RSDK.MatrixTranslateXYZ(&self->matrix2, self->position.x, self->height, self->position.y, true);
            RSDK.MatrixRotateZ(&self->matrix1, self->angleZ >> 5);
            RSDK.MatrixRotateY(&self->matrix3, self->angle + (self->angleZ >> (6 - (uint8)(UFO_Setup->machLevel))));
            RSDK.MatrixMultiply(&self->matrix4, &self->matrix1, &self->matrix3);
            RSDK.MatrixMultiply(&self->matrix3, &self->matrix4, &self->matrix2);
            RSDK.MatrixMultiply(&self->matrix3, &self->matrix3, &UFO_Camera->matWorld);
            RSDK.MatrixRotateXYZ(&self->matrix4, 0, self->angle, 0);
            RSDK.MatrixMultiply(&self->matrix4, &self->matrix4, &UFO_Camera->matView);
            RSDK.AddMeshFrameTo3DScene(self->animator.animationID, UFO_Player->sceneIndex, &self->animator, S3D_FLATCLR_SHADED_BLENDED_SCREEN,
                                       &self->matrix3, &self->matrix4, 0xFFFFFF);
        }
        RSDK.Draw3DScene(UFO_Player->sceneIndex);
    }
}

void UFO_Player_Create(void *data)
{
    RSDK_THIS(UFO_Player);
    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_NORMAL;
        self->visible       = true;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        self->drawFX        = FX_FLIP;
        self->drawOrder     = 4;
        if (!self->machQuota1)
            self->machQuota1 = 32;
        if (!self->machQuota2)
            self->machQuota2 = 32;
        if (!self->machQuota3)
            self->machQuota3 = 32;
        if (self->startingRings > 0)
            UFO_Setup->rings = self->startingRings;

        UFO_Setup->machQuotas[0] = self->machQuota1;
        UFO_Setup->machQuotas[1] = self->machQuota2;
        UFO_Setup->machQuotas[2] = self->machQuota3;
        self->angle            = 0x200;
        self->onGround         = true;
        UFO_Player_ChangeMachState();
        self->stateInput   = UFO_Player_ProcessPlayerControl;
        self->controllerID = CONT_ANY;
        self->state        = UFO_Player_StateRun;
        RSDK.SetModelAnimation(UFO_Player->jogModel, &self->animator, 128, 0, true, 0);
    }
}

void UFO_Player_StageLoad(void)
{
    if (globals->playerID == ID_NONE) {
        globals->playerID = ID_DEFAULT_PLAYER;
    }

    switch (globals->playerID & 0xFF) {
        case ID_TAILS:
            UFO_Player->jogModel    = RSDK.LoadMesh("Special/TailsJog.bin", SCOPE_STAGE);
            UFO_Player->dashModel   = RSDK.LoadMesh("Special/TailsDash.bin", SCOPE_STAGE);
            UFO_Player->jumpModel   = RSDK.LoadMesh("Special/TailsJump.bin", SCOPE_STAGE);
            UFO_Player->ballModel   = RSDK.LoadMesh("Special/TailsBall.bin", SCOPE_STAGE);
            UFO_Player->tumbleModel = RSDK.LoadMesh("Special/TailsTumble.bin", SCOPE_STAGE);
            break;
        case ID_KNUCKLES:
            UFO_Player->jogModel    = RSDK.LoadMesh("Special/KnuxJog.bin", SCOPE_STAGE);
            UFO_Player->dashModel   = RSDK.LoadMesh("Special/KnuxDash.bin", SCOPE_STAGE);
            UFO_Player->jumpModel   = RSDK.LoadMesh("Special/KnuxJump.bin", SCOPE_STAGE);
            UFO_Player->ballModel   = RSDK.LoadMesh("Special/KnuxBall.bin", SCOPE_STAGE);
            UFO_Player->tumbleModel = RSDK.LoadMesh("Special/KnuxTumble.bin", SCOPE_STAGE);
            break;
#if RETRO_USE_PLUS
        case ID_MIGHTY:
            UFO_Player->jogModel    = RSDK.LoadMesh("Special/MightyJog.bin", SCOPE_STAGE);
            UFO_Player->dashModel   = RSDK.LoadMesh("Special/MightyDash.bin", SCOPE_STAGE);
            UFO_Player->jumpModel   = RSDK.LoadMesh("Special/MightyJump.bin", SCOPE_STAGE);
            UFO_Player->ballModel   = RSDK.LoadMesh("Special/MightyBall.bin", SCOPE_STAGE);
            UFO_Player->tumbleModel = RSDK.LoadMesh("Special/MightyTumble.bin", SCOPE_STAGE);
            break;
        case ID_RAY:
            UFO_Player->jogModel    = RSDK.LoadMesh("Special/RayJog.bin", SCOPE_STAGE);
            UFO_Player->dashModel   = RSDK.LoadMesh("Special/RayDash.bin", SCOPE_STAGE);
            UFO_Player->jumpModel   = RSDK.LoadMesh("Special/RayJump.bin", SCOPE_STAGE);
            UFO_Player->ballModel   = RSDK.LoadMesh("Special/RayBall.bin", SCOPE_STAGE);
            UFO_Player->tumbleModel = RSDK.LoadMesh("Special/RayTumble.bin", SCOPE_STAGE);
            break;
#endif
        default:
            UFO_Player->jogModel    = RSDK.LoadMesh("Special/SonicJog.bin", SCOPE_STAGE);
            UFO_Player->dashModel   = RSDK.LoadMesh("Special/SonicDash.bin", SCOPE_STAGE);
            UFO_Player->jumpModel   = RSDK.LoadMesh("Special/SonicJump.bin", SCOPE_STAGE);
            UFO_Player->ballModel   = RSDK.LoadMesh("Special/SonicBall.bin", SCOPE_STAGE);
            UFO_Player->tumbleModel = RSDK.LoadMesh("Special/SonicTumble.bin", SCOPE_STAGE);
            break;
    }

    UFO_Player->sceneIndex = RSDK.Create3DScene("View:Special", 4096, SCOPE_STAGE);
    RSDK.SetDiffuseColour(UFO_Player->sceneIndex, 160, 160, 160);
    RSDK.SetDiffuseIntensity(UFO_Player->sceneIndex, 8, 8, 8);
    RSDK.SetSpecularIntensity(UFO_Player->sceneIndex, 15, 15, 15);

    foreach_all(UFO_Player, player)
    {
        EntityUFO_Player *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, UFO_Player);
        RSDK.CopyEntity(player1, player, true);
    }

    UFO_Player->sfxJump         = RSDK.GetSFX("Global/Jump.wav");
    UFO_Player->sfxCharge       = RSDK.GetSFX("Global/Charge.wav");
    UFO_Player->sfxRelease      = RSDK.GetSFX("Global/Release.wav");
    UFO_Player->sfxLoseRings    = RSDK.GetSFX("Global/LoseRings.wav");
    UFO_Player->sfxDrop         = RSDK.GetSFX("Stage/Drop.wav");
    UFO_Player->sfxBumper       = RSDK.GetSFX("Stage/Bumper.wav");
    UFO_Player->sfxSpring       = RSDK.GetSFX("Global/Spring.wav");
    UFO_Player->sfxSkid         = RSDK.GetSFX("Special/Skid.wav");
    UFO_Player->sfxGrittyGround = RSDK.GetSFX("Special/GrittyGround.wav");

    EntityUFO_Player *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, UFO_Player);
    RSDK.ClearCameras();
    RSDK.AddCamera(&player1->position, 0x100000, 0x100000, true);
}

void UFO_Player_ProcessPlayerControl(void)
{
    RSDK_THIS(UFO_Player);
    if (self->controllerID < PLAYER_MAX) {
#if RETRO_USE_TOUCH_CONTROLS
        for (int32 t = 0; t < TouchInfo->count; ++t) {
            int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
            int32 ty = (TouchInfo->y[t] * ScreenInfo->height);

            if (TouchInfo->down[t]) {
                if (tx >= 0 && ty >= 96 && tx <= ScreenInfo->centerX && ty <= ScreenInfo->height) {
                    int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
                    int32 ty = (TouchInfo->y[t] * ScreenInfo->height);
                    tx -= 64;
                    ty -= 192;

                    switch (((RSDK.ATan2(tx, ty) + 32) & 0xFF) >> 6) {
                        case 0:
                            ControllerInfo->keyRight.down |= true;
                            ControllerInfo[self->controllerID].keyRight.down = true;
                            break;
                        case 1:
                            ControllerInfo->keyDown.down |= true;
                            ControllerInfo[self->controllerID].keyDown.down = true;
                            break;
                        case 2:
                            ControllerInfo->keyLeft.down |= true;
                            ControllerInfo[self->controllerID].keyLeft.down = true;
                            break;
                        case 3:
                            ControllerInfo->keyUp.down |= true;
                            ControllerInfo[self->controllerID].keyUp.down = true;
                            break;
                    }
                    break;
                }
            }
        }

        for (int32 t = 0; t < TouchInfo->count; ++t) {
            int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
            int32 ty = (TouchInfo->y[t] * ScreenInfo->height);

            if (TouchInfo->down[t]) {
                if (tx >= ScreenInfo->centerX && ty >= 96 && tx <= ScreenInfo->width && ty <= ScreenInfo->height) {
                    ControllerInfo->keyA.down |= true;
                    ControllerInfo[self->controllerID].keyA.down = true;
                    break;
                }
            }
        }

        if (!self->touchJump) {
            ControllerInfo->keyA.press |= ControllerInfo->keyA.down;
            ControllerInfo[self->controllerID].keyA.press |= ControllerInfo[self->controllerID].keyA.down;
        }
        self->touchJump = ControllerInfo[self->controllerID].keyA.down;

        for (int32 t = 0; t < TouchInfo->count; ++t) {
            int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
            int32 ty = (TouchInfo->y[t] * ScreenInfo->height);

            if (TouchInfo->down[t]) {
                if (tx >= ScreenInfo->width - 0x80 && ty >= 0 && tx <= ScreenInfo->width && ty <= 0x40) {
                    if (SceneInfo->state == ENGINESTATE_REGULAR) {
                        EntityPauseMenu *pauseMenu = RSDK.GetEntityByID(SLOT_PAUSEMENU);
                        if (!pauseMenu->objectID) {
                            RSDK.ResetEntitySlot(SLOT_PAUSEMENU, PauseMenu->objectID, NULL);
                            pauseMenu->triggerPlayer = RSDK.GetEntityID(self);
                            pauseMenu->disableRestart = true;
                        }
                    }
                    break;
                }
            }
        }
#endif

        RSDKControllerState *controller = &ControllerInfo[self->controllerID];
        self->up                  = controller->keyUp.down;
        self->down                = controller->keyDown.down;
        self->left                = controller->keyLeft.down;
        self->right               = controller->keyRight.down;

        self->up |= AnalogStickInfoL[self->controllerID].keyUp.down;
        self->down |= AnalogStickInfoL[self->controllerID].keyDown.down;
        self->left |= AnalogStickInfoL[self->controllerID].keyLeft.down;
        self->right |= AnalogStickInfoL[self->controllerID].keyRight.down;
        self->up |= AnalogStickInfoL[self->controllerID].vDelta > 0.3;
        self->down |= AnalogStickInfoL[self->controllerID].vDelta < -0.3;
        self->left |= AnalogStickInfoL[self->controllerID].hDelta < -0.3;
        self->right |= AnalogStickInfoL[self->controllerID].hDelta > 0.3;

        if (self->left && self->right) {
            self->left  = false;
            self->right = false;
        }
        self->jumpPress = controller->keyA.press || controller->keyB.press || controller->keyC.press || controller->keyX.press;
        self->jumpHold  = controller->keyA.down || controller->keyB.down || controller->keyC.down || controller->keyX.down;

#if RETRO_USE_PLUS
        if (controller->keyStart.press || UnknownInfo->field_10 == 1) {
#else
        if (controller->keyStart.press) {
#endif

            if (SceneInfo->state == ENGINESTATE_REGULAR) {
                EntityPauseMenu *pauseMenu = RSDK.GetEntityByID(SLOT_PAUSEMENU);
                if (!pauseMenu->objectID) {
                    RSDK.ResetEntitySlot(SLOT_PAUSEMENU, PauseMenu->objectID, NULL);
                    pauseMenu->triggerPlayer  = RSDK.GetEntityID(self);
                    pauseMenu->disableRestart = true;
                }
            }
        }
    }
}

void UFO_Player_ChangeMachState(void)
{
    RSDK_THIS(UFO_Player);
    switch (UFO_Setup->machLevel) {
        default: break;
        case 0:
            UFO_Player->maxSpeed = 0x70000;
            self->field_84     = 16;
            self->field_8C     = 128;
            break;
        case 1:
            UFO_Player->maxSpeed = 0xA0000;
            self->field_84     = 18;
            self->field_8C     = 144;
            break;
        case 2:
            UFO_Player->maxSpeed = 0xE0000;
            self->field_84     = 20;
            self->field_8C     = 160;
            break;
    }
}

void UFO_Player_HandleBumperTiles(void)
{
    RSDK_THIS(UFO_Player);
    int32 flags = 0;

    uint16 tile = RSDK.GetTileInfo(UFO_Setup->playFieldLayer, (self->position.x - 0x80000) >> 20, (self->position.y - 0x80000) >> 20);
    if (RSDK.GetTileBehaviour(tile, 0) == 1)
        flags = 1;

    tile = RSDK.GetTileInfo(UFO_Setup->playFieldLayer, (self->position.x + 0x80000) >> 20, (self->position.y - 0x80000) >> 20);
    if (RSDK.GetTileBehaviour(tile, 0) == 1)
        flags |= 2;

    tile = RSDK.GetTileInfo(UFO_Setup->playFieldLayer, (self->position.x - 0x80000) >> 20, (self->position.y + 0x80000) >> 20);
    if (RSDK.GetTileBehaviour(tile, 0) == 1)
        flags |= 4;

    tile = RSDK.GetTileInfo(UFO_Setup->playFieldLayer, (self->position.x + 0x80000) >> 20, (self->position.y + 0x80000) >> 20);
    if (RSDK.GetTileBehaviour(tile, 0) == 1)
        flags |= 8;

    if (flags) {
        if (!self->bumperTimer)
            RSDK.PlaySfx(UFO_Player->sfxBumper, 0, 255);
        self->bumperTimer = 16;
        switch (flags) {
            case 1:
            case 6:
            case 7:
                self->velocity.y = 0x40000;
                self->velocity.x = 0x40000;
                break;
            case 2:
            case 9:
            case 11:
                self->velocity.x = -0x40000;
                self->velocity.y = 0x40000;
                break;
            case 3: self->velocity.y = 0x40000; break;
            case 4:
            case 13:
                self->velocity.y = -0x40000;
                self->velocity.x = 0x40000;
                break;
            case 5: self->velocity.x = 0x40000; break;
            case 8:
            case 14:
            case 15:
                self->velocity.x = -0x40000;
                self->velocity.y = -0x40000;
                break;
            case 10: self->velocity.x = -0x40000; break;
            case 12: self->velocity.y = -0x40000; break;
            default: break;
        }
    }
}

void UFO_Player_HandleSpeedUp(void)
{
    RSDK_THIS(UFO_Player);

    uint16 tile = RSDK.GetTileInfo(UFO_Setup->playFieldLayer, self->position.x >> 20, self->position.y >> 20);
    if (tile == 0xFFFF) {
        if (++self->field_70 >= 2) {
            self->interaction   = 0;
            self->timer         = 0;
            self->state         = UFO_Player_Unknown9;
            self->camera->state = UFO_Camera_Unknown3;

            if (!UFO_Setup->timedOut)
                RSDK.PlaySfx(UFO_Player->sfxDrop, 0, 255);
            return;
        }
    }
    else {
        self->field_70 = 0;
    }

    if (self->bumperTimer <= 0) {
        if (RSDK.GetTileBehaviour(tile, 0) == 2) {
            if (self->groundVel > UFO_Player->maxSpeed - (UFO_Player->maxSpeed >> 2))
                self->groundVel = self->groundVel - (self->groundVel >> 5);

            if (!(UFO_Setup->timer & 3)) {
                RSDK.CreateEntity(UFO_Dust->objectID, 0, self->position.x, self->position.y);
                RSDK.PlaySfx(UFO_Player->sfxGrittyGround, 0, 255);
            }
        }
        else {
            if (self->groundVel > UFO_Player->maxSpeed) {
                self->groundVel -= 0x4000;
                if (self->groundVel < UFO_Player->maxSpeed)
                    self->groundVel = UFO_Player->maxSpeed;
            }
            else if (self->groundVel < UFO_Player->maxSpeed) {
                self->groundVel += 0x4000;
                if (self->groundVel > UFO_Player->maxSpeed)
                    self->groundVel = UFO_Player->maxSpeed;
            }
        }
    }
}

void UFO_Player_StateRun(void)
{
    RSDK_THIS(UFO_Player);
    int32 val = UFO_Player->maxSpeed - 0x87000;
    if (self->right) {
        if (self->angleZ < (val >> 11) + 1280)
            self->angleZ += (val >> 14) + self->field_8C;
    }
    else if (self->left) {
        if (self->angleZ > -1280 - (val >> 11))
            self->angleZ -= self->field_8C - (val >> 14);
    }
    else {
        self->angleZ -= self->angleZ >> 2;
    }

    if (self->angleZ <= 0)
        self->angle -= -self->angleZ >> 8;
    else
        self->angle += self->angleZ >> 8;
    self->angle &= 0x3FF;

    if (abs(self->angleZ) <= 1280 || UFO_Setup->machLevel <= 1) {
        self->field_90 = 0;
    }
    else {
        if (self->field_90 >= 16) {
            if (!(UFO_Setup->timer & 3)) {
                RSDK.CreateEntity(UFO_Dust->objectID, 0, self->position.x, self->position.y);
            }
            if (!(UFO_Setup->timer & 7))
                RSDK.PlaySfx(UFO_Player->sfxSkid, 0, 255);
        }
        else {
            self->field_90++;
        }
    }

    if (self->bumperTimer >= 1) {
        self->bumperTimer--;
        UFO_Player_HandleSpeedUp();
    }
    else {
        UFO_Player_HandleSpeedUp();
        int32 x = (self->groundVel >> 10) * RSDK.Sin1024(self->angle);
        int32 y = (self->groundVel >> 10) * RSDK.Cos1024(self->angle);
        self->velocity.x += (x - self->velocity.x) / self->field_84;
        self->velocity.y += (-y - self->velocity.y) / self->field_84;
    }
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;

    if (!self->jumpPress || self->state == UFO_Player_Unknown9) {
        if (self->groundVel <= 0xC0000) {
            RSDK.SetModelAnimation(UFO_Player->jogModel, &self->animator, 128, 0, 0, 0);
            self->animator.animationSpeed = (self->groundVel >> 12) + 48;
        }
        else {
            RSDK.SetModelAnimation(UFO_Player->dashModel, &self->animator, 160, 0, 0, 0);
        }
        UFO_Player_HandleBumperTiles();
    }
    else {
        self->gravityStrength = 0x50000;
        self->onGround        = 0;
        self->state           = UFO_Player_StateJump;
        RSDK.SetModelAnimation(UFO_Player->jumpModel, &self->animator, 128, 0, true, 0);
        RSDK.PlaySfx(UFO_Player->sfxJump, 0, 255);
        UFO_Player_HandleBumperTiles();
    }
}

void UFO_Player_StateJump(void)
{
    RSDK_THIS(UFO_Player);
    int32 val = UFO_Player->maxSpeed - 0x60000;

    int32 speed = 0;
    if (self->right) {
        speed = (val >> 20) + 12;
        if (self->angleZ < (val >> 10) + 1280)
            self->angleZ += (self->field_8C >> 1) + (val >> 14);
    }
    else if (self->left) {
        speed = -12 - (val >> 20);
        if (self->angleZ > -1280 - (val >> 10))
            self->angleZ -= (self->field_8C >> 1) - (val >> 14);
    }
    else {
        speed = 0;
        self->angleZ -= self->angleZ >> 4;
    }

    if (self->angleZ <= 0)
        self->angle -= -self->angleZ >> 8;
    else
        self->angle += self->angleZ >> 8;
    self->angle &= 0x3FF;

    self->velocity.x -= self->velocity.x >> 8;
    self->velocity.y -= self->velocity.y >> 8;

    self->velocity.x += speed * RSDK.Cos1024(self->angle);
    self->velocity.y += speed * RSDK.Sin1024(self->angle);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->gravityStrength += -0x3800 - (val >> 7);

    self->height += self->gravityStrength;
    self->field_74 = (self->field_74 - (UFO_Player->maxSpeed >> 13)) & 0x3FF;
    if (self->field_74 & 0x100)
        self->animator.animationID = UFO_Player->ballModel;
    else
        self->animator.animationID = UFO_Player->jumpModel;

    if (self->height < 0 && self->gravityStrength <= 0) {
        self->field_70 = 4;
        self->height   = 0;
        self->onGround = true;
        self->state    = UFO_Player_StateRun;
        self->angleZ   = self->angleZ - (self->angleZ >> 1);
    }
    UFO_Player_HandleBumperTiles();
}

void UFO_Player_HandleTilt(void)
{
    RSDK_THIS(UFO_Player);

    int32 tilt = 0;
    if (self->right) {
        tilt = 8;
    }
    else if (self->left) {
        tilt = -8;
    }

    self->velocity.x -= self->velocity.x >> 8;
    self->velocity.y -= self->velocity.y >> 8;
    self->velocity.x += tilt * RSDK.Cos1024(self->angle);
    self->gravityStrength -= 0x5000;
    self->velocity.y += tilt * RSDK.Sin1024(self->angle);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->height += self->gravityStrength;

    self->field_74 = (self->field_74 - (UFO_Player->maxSpeed >> 13)) & 0x3FF;
    if (self->field_74 & 0x100)
        self->animator.animationID = UFO_Player->ballModel;
    else
        self->animator.animationID = UFO_Player->jumpModel;

    if (self->height < 0) {
        self->field_70 = 4;
        self->height   = 0;
        self->onGround = true;
        self->state    = UFO_Player_StateRun;
        self->angleZ   = self->angleZ - (self->angleZ >> 1);
    }
    UFO_Player_HandleBumperTiles();
}

void UFO_Player_Unknown8(void)
{
    RSDK_THIS(UFO_Player);
    int32 xVel = self->velocity.x;
    int32 zVel = self->velocity.y;
    self->position.x += xVel;
    self->position.y += zVel;
    self->velocity.x = xVel - (xVel >> 8);
    self->velocity.y = zVel - (zVel >> 8);

    if (!self->onGround) {
        self->gravityStrength -= 0x3800;
        self->height += self->gravityStrength;
        if (self->height < 0) {
            self->height   = 0;
            self->onGround = true;
        }
    }

    UFO_Player_HandleBumperTiles();
    if (++self->timer > 32) {
        self->timer = 0;
        if (self->onGround)
            self->state = UFO_Player_StateRun;
        else
            self->state = UFO_Player_StateJump;
    }
}

void UFO_Player_Unknown9(void)
{
    RSDK_THIS(UFO_Player);
    int32 x = self->velocity.x;
    int32 y = self->velocity.y;
    self->gravityStrength -= 0x800;
    self->position.x += x - (x >> 5);
    self->velocity.x = x - (x >> 5);
    self->position.y += y - (y >> 5);
    self->height += self->gravityStrength;
    self->velocity.y = y - (y >> 5);
    if (self->height < 0)
        self->drawOrder = 2;
    if (RSDK_GET_ENTITY(SLOT_UFO_SETUP, UFO_Setup)->state == UFO_Setup_Unknown13) {
        ++self->timer;
        if (self->timer == 120) {
            UFO_Setup_Finish_Fail();
        }
        else if (self->timer == 16) {
            if (!UFO_Setup->timedOut) {
                UFO_Setup->timedOut = true;
                RSDK.CreateEntity(UFO_Message->objectID, (void *)2, self->position.x, self->position.y);
            }
        }
    }
}

void UFO_Player_Unknown10(void)
{
    RSDK_THIS(UFO_Player);
    int32 xVel = self->velocity.x;
    int32 zVel = self->velocity.y;
    ++self->timer;
    self->position.x += xVel - (xVel >> 4);
    self->velocity.x = xVel - (xVel >> 4);
    self->position.y += zVel - (zVel >> 4);
    self->velocity.y = zVel - (zVel >> 4);

    self->field_74 = (self->field_74 - (UFO_Player->maxSpeed >> 13)) & 0x3FF;
    if (self->timer == 2) {
        RSDK.SetModelAnimation(UFO_Player->jumpModel, &self->animator, 128, 0, true, 0);
        RSDK.PlaySfx(UFO_Player->sfxCharge, 0, 255);
    }
    if (self->timer >= 30) {
        self->timer           = 0;
        self->camera->state   = UFO_Camera_Unknown4;
        self->velocity.x      = (self->circuitPtr->position.x - self->position.x) >> 4;
        self->velocity.y      = (self->circuitPtr->position.y - self->position.y) >> 4;
        self->state           = UFO_Player_Unknown11;
        self->gravityStrength = (self->circuitPtr->height - self->height + 0x200000) >> 4;
        RSDK.PlaySfx(UFO_Player->sfxRelease, 0, 255);
    }
}

void UFO_Player_Unknown11(void)
{
    RSDK_THIS(UFO_Player);
    self->position.x += self->velocity.x;
    self->gravityStrength -= 128;
    self->position.y += self->velocity.y;
    self->height += self->gravityStrength;
    ++self->timer;

    self->field_74 = (self->field_74 - (UFO_Player->maxSpeed >> 13)) & 0x3FF;
    if (self->timer == 16)
        UFO_Setup_Finish_Win();
}

void UFO_Player_EditorDraw(void) {}

void UFO_Player_EditorLoad(void) { }

void UFO_Player_Serialize(void)
{
    RSDK_EDITABLE_VAR(UFO_Player, VAR_ENUM, machQuota1);
    RSDK_EDITABLE_VAR(UFO_Player, VAR_ENUM, machQuota2);
    RSDK_EDITABLE_VAR(UFO_Player, VAR_ENUM, machQuota3);
    RSDK_EDITABLE_VAR(UFO_Player, VAR_ENUM, startingRings);
}

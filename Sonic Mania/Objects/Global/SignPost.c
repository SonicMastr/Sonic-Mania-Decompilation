#include "SonicMania.h"

ObjectSignPost *SignPost;

void SignPost_Update(void)
{
    RSDK_THIS(SignPost);
    StateMachine_Run(self->state);
}

void SignPost_LateUpdate(void) {}

void SignPost_StaticUpdate(void) {}

void SignPost_Draw(void)
{
    RSDK_THIS(SignPost);
    Vector2 drawPos;
    if (self->state) {
        self->drawFX = FX_SCALE;
        drawPos.y      = self->position.y;

        Animator *face = &self->facePlateAnim;
        if (self->rotation <= 128 || self->rotation >= 384)
            face = &self->eggPlateAnim;

        self->scale.x = abs(RSDK.Cos512(self->rotation));
        int32 scale = abs(RSDK.Sin512(self->rotation));

        switch (self->rotation >> 7) {
            case 0:
            case 2:
                drawPos.x = self->position.x + (scale << 9);
                RSDK.DrawSprite(face, &drawPos, false);
                drawPos.x += -0xC00 * self->scale.x - (scale << 9);
                break;
            case 1:
            case 3:
                drawPos.x = self->position.x - (scale << 9);
                RSDK.DrawSprite(face, &drawPos, false);
                drawPos.x += (scale + 2 * (3 * self->scale.x - 32)) << 9;
                break;
            default: break;
        }
        self->scale.x = scale;
        RSDK.DrawSprite(&self->sidebarData, &drawPos, false);

        self->drawFX = FX_NONE;
        RSDK.DrawSprite(&self->postTopData, NULL, false);
        RSDK.DrawSprite(&self->standData, NULL, false);
    }
}

void SignPost_Create(void *data)
{
    RSDK_THIS(SignPost);

    if (!self->vsBoundsSize.x)
        self->vsBoundsSize.x = 0x2000000;
    if (!self->vsBoundsSize.y)
        self->vsBoundsSize.y = 0xF00000;
    if (!self->vsExtendTop)
        self->vsExtendTop = 120;
    if (!self->vsExtendBottom)
        self->vsExtendBottom = 120;

    if (!SceneInfo->inEditor) {
        if (globals->gameMode != MODE_TIMEATTACK) {
            RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_EGGMAN, &self->eggPlateAnim, true, 0);
            switch (globals->playerID & 0xFF) {
                case ID_TAILS: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_TAILS, &self->facePlateAnim, true, 0); break;
                case ID_KNUCKLES: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_KNUX, &self->facePlateAnim, true, 0); break;
#if RETRO_USE_PLUS
                case ID_MIGHTY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_MIGHTY, &self->facePlateAnim, true, 0); break;
                case ID_RAY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_RAY, &self->facePlateAnim, true, 0); break;
#endif
                default: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_SONIC, &self->facePlateAnim, true, 0); break;
            }
            RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &self->postTopData, true, 0);
            RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &self->sidebarData, true, 1);
            RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &self->standData, true, 2);
            self->updateRange.x = 0x400000;
            self->updateRange.y = 0x400000;
            if (globals->gameMode == MODE_COMPETITION) {
                int32 checkA = self->vsBoundsOffset.y + (self->vsBoundsSize.y >> 1);
                int32 checkB = self->vsBoundsOffset.y - (self->vsBoundsSize.y >> 1);

                if ((-0x10000 * self->vsExtendTop) < self->vsBoundsOffset.y - (self->vsBoundsSize.y >> 1))
                    checkB = (-0x10000 * self->vsExtendTop);

                if (self->vsExtendBottom << 16 < checkA)
                    checkA = self->vsExtendBottom << 16;

                if (abs(self->vsBoundsOffset.x - (self->vsBoundsSize.x >> 1)) > checkB)
                    self->updateRange.x = abs(self->vsBoundsOffset.x - (self->vsBoundsSize.x >> 1));
                else
                    self->updateRange.x = ((self->vsBoundsSize.x >> 1) + self->vsBoundsOffset.x) + 0x400000;

                if (abs(checkB) > checkA)
                    self->updateRange.y = abs(checkB) + 0x400000;
                else
                    self->updateRange.y = checkA + 0x400000;
            }
            self->visible   = true;
            self->drawOrder = Zone->drawOrderLow;
            self->spinSpeed = 0x3000;
            self->spinCount = 8;
            self->maxAngle  = 0x10000;
            self->scale.y   = 0x200;

            switch (self->type) {
                default: destroyEntity(self); break;
                case 0:
                    if (globals->gameMode != MODE_COMPETITION) {
                        self->active = ACTIVE_BOUNDS;
                        self->state  = SignPost_State_SetupCompetition;
                    }
                    else {
                        destroyEntity(self);
                    }
                    break;
                case 1:
                    if (globals->gameMode != MODE_COMPETITION) {
                        self->active = ACTIVE_XBOUNDS;
                        self->state  = StateMachine_None;
                    }
                    else {
                        destroyEntity(self);
                    }
                    break;
                case 2:
                    if (globals->gameMode == MODE_COMPETITION) {
                        self->active = ACTIVE_BOUNDS;
                        self->state  = SignPost_State_SetupCompetition;
                    }
                    else {
                        destroyEntity(self);
                    }
                    break;
                case 3:
                    if (globals->gameMode != MODE_COMPETITION) {
                        self->active = ACTIVE_BOUNDS;
                        self->state  = SignPost_State_Finish;
                    }
                    else {
                        destroyEntity(self);
                    }
                    break;
                case 4:
                    if (globals->gameMode != MODE_COMPETITION) {
                        self->active = ACTIVE_BOUNDS;
                        self->state  = SignPost_State_SetupCompetition;
                    }
                    else {
                        destroyEntity(self);
                    }
                    break;
            }
        }
    }
}

void SignPost_StageLoad(void)
{
    SignPost->aniFrames          = RSDK.LoadSpriteAnimation("Global/SignPost.bin", SCOPE_STAGE);
    SignPost->hitbox.left          = -24;
    SignPost->hitbox.top           = -22;
    SignPost->hitbox.right         = 24;
    SignPost->hitbox.bottom        = 8;
    SignPost->itemBoxHitbox.left   = -8;
    SignPost->itemBoxHitbox.top    = 20;
    SignPost->itemBoxHitbox.right  = 8;
    SignPost->itemBoxHitbox.bottom = 24;
    SignPost->maxPlayerCount       = (1 << Player->playerCount) - 1;

    DEBUGMODE_ADD_OBJ(SignPost);
    SignPost->sfxSignPost     = RSDK.GetSFX("Global/SignPost.wav");
    SignPost->sfxSignPost2P   = RSDK.GetSFX("Global/SignPost2p.wav");
    SignPost->sfxTwinkle      = RSDK.GetSFX("Global/Twinkle.wav");
    SignPost->sfxBubbleBounce = RSDK.GetSFX("Global/BubbleBounce.wav");
    SignPost->sfxSlide        = RSDK.GetSFX("Global/Slide.wav");
}

void SignPost_DebugSpawn(void)
{
    RSDK_THIS(SignPost);
    EntitySignPost *signpost = CREATE_ENTITY(SignPost, NULL, self->position.x, self->position.y);
    signpost->debugObj       = true;
}
void SignPost_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_EGGMAN, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &DebugMode->animator, true, 2);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}
void SignPost_SpinSpeed(void)
{
    RSDK_THIS(SignPost);
    self->angle += self->spinSpeed;

    if (self->angle >= self->maxAngle) {
        self->maxAngle += 0x20000;
        int32 speed = 0x600 * (self->spinCount);
        if (speed > 0x3000)
            speed = 0x3000;
        self->spinSpeed = speed;
        --self->spinCount;
        if (!self->spinCount) {
            self->spinSpeed                    = 0;
            self->angle                        = 0x10000;
            self->facePlateAnim.animationSpeed = 1;
        }
    }
    self->rotation = (self->angle >> 8) & 0x1FF;
}
void SignPost_SpawnSparkle(void)
{
    RSDK_THIS(SignPost);
    if (!(Zone->timer & 3)) {
        EntityRing *ring =
            CREATE_ENTITY(Ring, 0, self->position.x + RSDK.Rand(-0x180000, 0x180000), self->position.y + RSDK.Rand(-0x200000, 0x80000));
        ring->state      = Ring_State_Sparkle;
        ring->stateDraw  = Ring_Draw_Sparkle;
        ring->active     = ACTIVE_NORMAL;
        ring->visible    = false;
        RSDK.SetSpriteAnimation(Ring->aniFrames, self->sparkleType + 2, &ring->animator, true, 0);
        int32 cnt = ring->animator.frameCount;
        if (ring->animator.animationID == 2) {
            ring->alpha = 224;
            cnt >>= 1;
        }
        ring->maxFrameCount           = cnt - 1;
        ring->animator.animationSpeed = 6;
        self->sparkleType           = (self->sparkleType + 1) % 3;
    }
}
void SignPost_State_SetupCompetition(void)
{
    RSDK_THIS(SignPost);
    self->state = SignPost_State_Competition;
}
void SignPost_State_Competition(void)
{
    SignPost_HandleCompetition();
    SignPost_CheckTouch();
}
void SignPost_State_Land(void)
{
    RSDK_THIS(SignPost);
    SignPost_SpinSpeed();
    SignPost_SpawnSparkle();
    RSDK.ProcessAnimation(&self->facePlateAnim);
    if (!self->spinCount) {
        self->type  = 3;
        self->state = SignPost_State_Finish;
        Music_PlayTrack(TRACK_ACTCLEAR);
        RSDK.ResetEntitySlot(SLOT_ACTCLEAR, ActClear->objectID, NULL);
    }
}
void SignPost_State_CompetitionFinish(void)
{
    RSDK_THIS(SignPost);
    SignPost_SpinSpeed();
    SignPost_SpawnSparkle();
    RSDK.ProcessAnimation(&self->facePlateAnim);
    SignPost_CheckTouch();

    if (!self->spinCount) {
        if (self->activePlayers >= SignPost->maxPlayerCount) {
            self->type                = 3;
            self->state               = SignPost_State_Finish;
            SceneInfo->timeEnabled = false;
            Zone_StartFadeOut(10, 0x000000);
        }
        else {
            self->spinSpeed = 0x3000;
            self->spinCount = 8;
            self->maxAngle  = 0x10000;
            self->state     = SignPost_State_Competition;
        }
    }
}
void SignPost_State_Launched(void)
{
    RSDK_THIS(SignPost);
    SignPost_SpinSpeed();
    SignPost_SpawnSparkle();
    RSDK.ProcessAnimation(&self->facePlateAnim);
    self->spinCount = 16;
    self->position.y += self->velocity.y;
    self->velocity.y += self->gravityStrength;
    if (self->velocity.y < 0)
        self->state = SignPost_State_Fall;
}
void SignPost_State_Fall(void)
{
    RSDK_THIS(SignPost);
    self->active              = ACTIVE_NORMAL;
    SceneInfo->timeEnabled = false;
    if (self->type == 1) {
        self->type = 0;
        if (globals->gameMode < MODE_COMPETITION) {
            switch (globals->playerID & 0xFF) {
                case ID_TAILS: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_TAILS, &self->facePlateAnim, true, 0); break;
                case ID_KNUCKLES: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_KNUX, &self->facePlateAnim, true, 0); break;
#if RETRO_USE_PLUS
                case ID_MIGHTY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_MIGHTY, &self->facePlateAnim, true, 0); break;
                case ID_RAY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_RAY, &self->facePlateAnim, true, 0); break;
#endif
                default: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_SONIC, &self->facePlateAnim, true, 0); break;
            }
        }
    }

    foreach_active(Player, player) {
        if (self->velocity.y >= 0) {
            if (player->velocity.y < 0 && player->animator.animationID == ANI_JUMP && !player->onGround) {
                if (Player_CheckCollisionTouch(player, self, &SignPost->hitbox)) {
                    self->velocity.x = (self->position.x - player->position.x) >> 4;
                    self->velocity.y = -0x20000;
                    RSDK.PlaySfx(SignPost->sfxTwinkle, 0, 255);
                    EntityScoreBonus *scoreBonus = CREATE_ENTITY(ScoreBonus, NULL, self->position.x, self->position.y);
                    scoreBonus->drawOrder    = Zone->drawOrderHigh;
                    scoreBonus->animator.frameID = 0;
                    Player_GiveScore(player, 100);
                }
            }
        }
    }

    SignPost_SpinSpeed();
    SignPost_SpawnSparkle();
    RSDK.ProcessAnimation(&self->facePlateAnim);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->spinCount = 16;

    if (self->velocity.x >= 0) {
        if (self->position.x > (ScreenInfo->position.x + ScreenInfo->width - 32) << 16) {
            self->velocity.x = -self->velocity.x;
        }
        else if (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_LWALL, 0, 0x180000, 0, true)) {
            self->velocity.x = -self->velocity.x;
        }
    }
    else {
        if (self->position.x < (ScreenInfo->position.x + 32) << 16) {
            self->velocity.x = -self->velocity.x;
        }
        else if (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_RWALL, 0, -0x180000, 0, true)) {
            self->velocity.x = -self->velocity.x;
        }
    }

    self->velocity.y += 0xC00;
    if (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x180000, true)) {
        foreach_active(ItemBox, itemBox) {
            if (itemBox->hidden) {
                if (RSDK.CheckObjectCollisionTouchBox(itemBox, &ItemBox->hiddenHitbox, self, &SignPost->itemBoxHitbox)) {
                    RSDK.PlaySfx(SignPost->sfxBubbleBounce, 0, 255);
                    itemBox->velocity.y = -0x50000;
                    itemBox->hidden     = 0;
                    itemBox->state      = ItemBox_State_Falling;
                    self->itemBounceCount++;
                    self->velocity.y = -0x20000;
                    if (self->itemBounceCount == 2)
                        API_UnlockAchievement("ACH_SIGNPOST");
                }
            }
        }
        if (self->velocity.y >= 0) {
            RSDK.PlaySfx(SignPost->sfxSlide, 0, 255);
            self->spinCount  = 4;
            self->velocity.y = 0;
            Music_FadeOut(0.025);
            self->state = SignPost_State_Land;
        }
    }
}
void SignPost_State_Finish(void)
{
    RSDK_THIS(SignPost);
    RSDK.ProcessAnimation(&self->facePlateAnim);
    if (self->debugObj) {
        Zone->stageFinishCallback  = StateMachine_None;
        globals->atlEnabled        = false;
        globals->enableIntro       = false;
        globals->suppressTitlecard = false;
        globals->suppressAutoMusic = false;
    }
}
void SignPost_CheckTouch(void)
{
    RSDK_THIS(SignPost);

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
        if (self->activePlayers && RSDK_GET_ENTITY(p + Player->playerCount, Player)->objectID == GameOver->objectID) {
            self->activePlayers |= 1 << p;
        }
        else if (!p || globals->gameMode == MODE_COMPETITION) {
            if (!((1 << p) & self->activePlayers)) {
                bool32 flag = false;
                if (globals->gameMode != MODE_COMPETITION) {
                    flag = player->position.x > self->position.x;
                }
                else if (self->playerPosStore[p].x && self->playerPosStore[p].y) {
                    flag = MathHelpers_Unknown12(player->position.x, player->position.y, self->playerPosStore[p].x, self->playerPosStore[p].y,
                                                 self->position.x, self->position.y - (self->vsExtendTop << 16), self->position.x,
                                                 (self->vsExtendBottom << 16) + self->position.y);
                }

                if (flag) {
                    if (!((1 << p) & self->activePlayers) && globals->gameMode == MODE_COMPETITION)
                        Announcer_AnnounceGoal(player->camera->screenID);
                    RSDK.PlaySfx(SignPost->sfxSignPost, 0, 255);
                    self->active = ACTIVE_NORMAL;
                    if (player->superState == SUPERSTATE_SUPER)
                        player->superState = SUPERSTATE_FADEOUT;

                    int32 vel = 0;
                    if (player->onGround)
                        vel = player->groundVel;
                    else
                        vel = player->velocity.x;

                    self->velocity.y      = -(vel >> 1);
                    self->gravityStrength = vel / 96;
                    if (globals->gameMode == MODE_COMPETITION) {
                        self->active = ACTIVE_NORMAL;
                        if (!self->activePlayers) {
                            switch (player->characterID) {
                                case ID_TAILS: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_TAILS, &self->facePlateAnim, true, 0); break;
                                case ID_KNUCKLES:
                                    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_KNUX, &self->facePlateAnim, true, 0);
                                    break;
#if RETRO_USE_PLUS
                                case ID_MIGHTY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_MIGHTY, &self->facePlateAnim, true, 0); break;
                                case ID_RAY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_RAY, &self->facePlateAnim, true, 0); break;
#endif
                                default: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_SONIC, &self->facePlateAnim, true, 0); break;
                            }
                            RSDK.PlaySfx(SignPost->sfxSignPost2P, 0, 255);
                        }

                        EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;
                        EntityCompetition *manager = (EntityCompetition *)Competition->activeEntity;
                        if (!manager) {
                            Competition->activeEntity = (Entity *)CREATE_ENTITY(Competition, NULL, self->position.x, self->position.y);
                        }
                        manager->playerFlags[player->playerID] = true;
                        session->rings[player->playerID]             = player->rings;
                        session->time[player->playerID].minutes      = SceneInfo->minutes;
                        session->time[player->playerID].seconds      = SceneInfo->seconds;
                        session->time[player->playerID].milliseconds = SceneInfo->milliseconds;
                        session->score[player->playerID]             = player->score;
                        session->lives[player->playerID]             = player->lives;
#if RETRO_USE_PLUS
                        Competition_CalculateScore(player->playerID, 2);
#else
                        CompetitionSession_DeriveWinner(player->playerID, 2);
#endif

                        self->activePlayers |= (1 << p);
                        if (self->activePlayers == SignPost->maxPlayerCount)
                            Music_FadeOut(0.025);
                        self->state = SignPost_State_CompetitionFinish;
                    }
                    else {
#if RETRO_USE_PLUS
                        if (globals->gameMode == MODE_ENCORE) {
                            switch (globals->playerID & 0xFF) {
                                case ID_TAILS: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_TAILS, &self->facePlateAnim, true, 0); break;
                                case ID_KNUCKLES:
                                    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_KNUX, &self->facePlateAnim, true, 0);
                                    break;
                                case ID_MIGHTY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_MIGHTY, &self->facePlateAnim, true, 0); break;
                                case ID_RAY: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_RAY, &self->facePlateAnim, true, 0); break;
                                default: RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_SONIC, &self->facePlateAnim, true, 0); break;
                            }
                        }
#endif

                        SceneInfo->timeEnabled = false;
                        if (vel >= 0x40000) {
                            self->state = SignPost_State_Launched;
                        }
                        else {
                            Music_FadeOut(0.025);
                            self->state = SignPost_State_Land;
                        }
                    }
                }
                self->playerPosStore[p].x = player->position.x;
                self->playerPosStore[p].y = player->position.y;
            }
        }
    }
}
void SignPost_HandleCompetition(void)
{
    RSDK_THIS(SignPost);
    int32 x = self->vsBoundsOffset.x + self->position.x;
    int32 y = self->vsBoundsOffset.y + self->position.y;

    Hitbox hitbox;
    hitbox.left   = -self->vsBoundsSize.x >> 17;
    hitbox.top    = -self->vsBoundsSize.y >> 17;
    hitbox.right  = self->vsBoundsSize.x >> 17;
    hitbox.bottom = self->vsBoundsSize.y >> 17;

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
        if (player->objectID == Player->objectID && !player->sidekick) {
            if (globals->gameMode == MODE_COMPETITION) {
                int32 ex             = self->position.x;
                int32 ey             = self->position.y;
                self->position.x = x;
                self->position.y = y;
                if (Player_CheckCollisionTouch(player, self, &hitbox)) {
                    self->position.x      = ex;
                    self->position.y      = ey;
                    Zone->screenBoundsL1[p] = (self->position.x >> 0x10) - ScreenInfo[p].centerX;
                    Zone->screenBoundsR1[p] = ScreenInfo[p].centerX + (self->position.x >> 0x10);
                    if (globals->gameMode == MODE_COMPETITION)
                        Zone->playerBoundActiveR[p] = true;
                }
                else {
                    self->position.x = ex;
                    self->position.y = ey;
                }
            }
            else {
                if (self->position.x - player->position.x < 0x1000000 || self->position.x - (Zone->screenBoundsR1[p] << 16) < 0x1000000) {
                    Zone->screenBoundsL1[p] = (self->position.x >> 0x10) - ScreenInfo[p].centerX;
                    Zone->screenBoundsR1[p] = ScreenInfo[p].centerX + (self->position.x >> 0x10);
                    if (globals->gameMode == MODE_COMPETITION)
                        Zone->playerBoundActiveR[p] = true;
                }
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void SignPost_EditorDraw(void)
{
    RSDK_THIS(SignPost);
    Vector2 drawPos;

    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_EGGMAN, &self->eggPlateAnim, true, 0);
    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &self->postTopData, true, 0);
    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &self->sidebarData, true, 1);
    RSDK.SetSpriteAnimation(SignPost->aniFrames, SIGNPOSTANI_POST, &self->standData, true, 2);

    self->drawFX = FX_SCALE;
    drawPos.y      = self->position.y;

    self->scale.x = abs(RSDK.Cos512(self->rotation));
    int32 scale     = abs(RSDK.Sin512(self->rotation));

    switch (self->rotation >> 7) {
        case 0:
        case 2:
            drawPos.x = self->position.x + (scale << 9);
            RSDK.DrawSprite(&self->eggPlateAnim, &drawPos, false);
            drawPos.x += -0xC00 * self->scale.x - (scale << 9);
            break;
        case 1:
        case 3:
            drawPos.x = self->position.x - (scale << 9);
            RSDK.DrawSprite(&self->eggPlateAnim, &drawPos, false);
            drawPos.x += (scale + 2 * (3 * self->scale.x - 32)) << 9;
            break;
        default: break;
    }
    self->scale.x = scale;
    RSDK.DrawSprite(&self->sidebarData, &drawPos, false);

    self->drawFX = FX_NONE;
    RSDK.DrawSprite(&self->postTopData, NULL, false);
    RSDK.DrawSprite(&self->standData, NULL, false);
}

void SignPost_EditorLoad(void) { SignPost->aniFrames = RSDK.LoadSpriteAnimation("Global/SignPost.bin", SCOPE_STAGE); }
#endif

void SignPost_Serialize(void)
{
    RSDK_EDITABLE_VAR(SignPost, VAR_UINT8, type);
    RSDK_EDITABLE_VAR(SignPost, VAR_VECTOR2, vsBoundsSize);
    RSDK_EDITABLE_VAR(SignPost, VAR_VECTOR2, vsBoundsOffset);
    RSDK_EDITABLE_VAR(SignPost, VAR_ENUM, vsExtendTop);
    RSDK_EDITABLE_VAR(SignPost, VAR_ENUM, vsExtendBottom);
}

// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: TargetBumper Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled By Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectTargetBumper *TargetBumper = NULL;

void TargetBumper_Update(void)
{
    RSDK_THIS(TargetBumper);
    StateMachine_Run(self->state);
}

void TargetBumper_LateUpdate(void) {}

void TargetBumper_StaticUpdate(void) {}

void TargetBumper_Draw(void)
{
    RSDK_THIS(TargetBumper);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void TargetBumper_Create(void *data)
{
    RSDK_THIS(TargetBumper);
    self->visible = true;
    self->drawFX |= FX_FLIP;
    self->drawOrder     = Zone->drawOrderLow;
    self->startPos      = self->position;
    self->active        = ACTIVE_BOUNDS;
    self->updateRange.x = 0x400000;
    self->updateRange.y = 0x400000;
    RSDK.SetSpriteAnimation(TargetBumper->aniFrames, self->type, &self->animator, true, 0);
    self->animator.frameID = self->hitCount;
    self->state            = TargetBumper_State_Collide;
}

void TargetBumper_StageLoad(void)
{
    TargetBumper->aniFrames = RSDK.LoadSpriteAnimation("Blueprint/TargetBumper.bin", SCOPE_STAGE);
    DEBUGMODE_ADD_OBJ(TargetBumper);
}

void TargetBumper_DebugSpawn(void)
{
    RSDK_THIS(TargetBumper);
    CREATE_ENTITY(TargetBumper, NULL, self->position.x, self->position.y);
}

void TargetBumper_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(TargetBumper->aniFrames, 0, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, 0, false);
}

void TargetBumper_Collide(void)
{
    RSDK_THIS(TargetBumper);

    Hitbox hitbox;
    switch (self->type) {
        case TARGETBUMP_HORIZONTAL:
            hitbox.left   = -14;
            hitbox.top    = -4;
            hitbox.right  = 14;
            hitbox.bottom = 4;
            break;
        case TARGETBUMP_VERTICAL:
            hitbox.left   = -4;
            hitbox.top    = -14;
            hitbox.right  = 4;
            hitbox.bottom = 14;
            break;
        case TARGETBUMP_CIRCLE:
            hitbox.left   = -8;
            hitbox.top    = -8;
            hitbox.right  = 8;
            hitbox.bottom = 8;
            break;
    }

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &hitbox) && player->animator.animationID != ANI_HURT) {
            self->curPos = self->startPos;
            self->state  = TargetBumper_Hit;
            self->active = ACTIVE_NORMAL;

            switch (self->type) {
                case TARGETBUMP_HORIZONTAL:
                    if (player->position.y <= self->position.y) {
                        player->velocity.y = -0x70000;
                        self->curPos.y += 0x20000;
                    }
                    else {
                        player->velocity.y = 0x70000;
                        self->curPos.y -= 0x20000;
                    }
                    break;
                case TARGETBUMP_VERTICAL:
                    if (player->position.x <= self->position.x) {
                        player->velocity.x = -0x70000;
                        self->curPos.x += 0x20000;
                    }
                    else {
                        player->velocity.x = 0x70000;
                        self->curPos.x -= 0x20000;
                    }
                    break;
                case TARGETBUMP_CIRCLE: {
                    int32 angle = 96;
                    if (self->direction)
                        angle = 32;

                    int32 ang2 = 0;
                    uint8 atan = RSDK.ATan2(player->velocity.x, player->velocity.y);
                    int32 ang  = atan - angle;
                    if (atan - angle >= 0)
                        ang2 = ang;
                    else
                        ang2 = -ang;

                    if (ang2 < 0x40) {
                        if (ang2 < 0x38) {
                            angle -= ang;
                            angle &= 0xFF;
                        }
                        if ((self->direction & 1))
                            self->curPos.x += 0x20000;
                        else
                            self->curPos.x -= 0x20000;
                        self->curPos.y += 0x20000;
                    }
                    else {
                        angle += 0x80;
                        if ((0x80 - ang2) < 0x38) {
                            angle -= ang;
                            angle &= 0xFF;
                        }
                        if ((self->direction & 1))
                            self->curPos.x -= 0x20000;
                        else
                            self->curPos.x += 0x20000;
                        self->curPos.y -= 0x20000;
                    }
                    player->velocity.x = -0x700 * RSDK.Cos256(angle);
                    player->velocity.y = -0x700 * RSDK.Sin256(angle);
                    break;
                }
            }

            if (player->state == Player_State_FlyCarried)
                RSDK_GET_ENTITY(SLOT_PLAYER2, Player)->flyCarryTimer = 30;

            int32 anim = player->animator.animationID;
            if (anim != ANI_FLY && anim != ANI_FLYLIFTTIRED && player->state != Player_State_TailsFlight) {
                player->state = Player_State_Air;
                if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                    player->animator.animationID = ANI_WALK;
            }
            if (player->animator.animationID != ANI_FLY)
                player->groundVel = player->velocity.x;
            player->onGround       = false;
            player->tileCollisions = true;
            if (self->hitCount < 3) {
                self->hitTimer        = 0;
                EntityScoreBonus *bonus = CREATE_ENTITY(ScoreBonus, NULL, self->position.x, self->position.y);
                bonus->animator.frameID = 16;
                Player_GiveScore(player, 10);
                if (++self->hitCount < 3)
                    self->animator.frameID = self->hitCount;
            }
        }
    }
}

void TargetBumper_State_Collide(void)
{
    // eminem ?
    TargetBumper_Collide();
}

void TargetBumper_Hit(void)
{
    RSDK_THIS(TargetBumper);
    TargetBumper_Collide();

    if ((self->hitTimer & 4)) {
        self->position.x = self->startPos.x;
        self->position.y = self->startPos.y;
    }
    else {
        self->position.x = self->curPos.x;
        self->position.y = self->curPos.y;
    }

    self->hitTimer++;
    if (self->hitTimer == 12) {
        if (self->hitCount < 3) {
            self->position.x = self->startPos.x;
            self->position.y = self->startPos.y;
            self->hitTimer   = 0;
            self->state      = TargetBumper_State_Collide;
        }
        else {
            destroyEntity(self);
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void TargetBumper_EditorDraw(void)
{
    RSDK_THIS(TargetBumper);
    RSDK.SetSpriteAnimation(TargetBumper->aniFrames, self->type, &self->animator, true, 0);
    self->animator.frameID = self->hitCount;
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void TargetBumper_EditorLoad(void)
{
    TargetBumper->aniFrames = RSDK.LoadSpriteAnimation("Blueprint/TargetBumper.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(TargetBumper, type);
    RSDK_ENUM_VAR("Horizontal", TARGETBUMP_HORIZONTAL);
    RSDK_ENUM_VAR("Vertical", TARGETBUMP_VERTICAL);
    RSDK_ENUM_VAR("Circular", TARGETBUMP_CIRCLE);

    RSDK_ACTIVE_VAR(TargetBumper, hitCount);
    RSDK_ENUM_VAR("Three Hits", TARGETBUMP_THREE_HIT);
    RSDK_ENUM_VAR("Two Hits", TARGETBUMP_TWO_HIT);
    RSDK_ENUM_VAR("One Hit", TARGETBUMP_ONE_HIT);

    RSDK_ACTIVE_VAR(TargetBumper, direction);
    RSDK_ENUM_VAR("No Flip", FLIP_NONE);
    RSDK_ENUM_VAR("Flip X", FLIP_X);
}
#endif

void TargetBumper_Serialize(void)
{
    RSDK_EDITABLE_VAR(TargetBumper, VAR_ENUM, type);
    RSDK_EDITABLE_VAR(TargetBumper, VAR_ENUM, hitCount);
    RSDK_EDITABLE_VAR(TargetBumper, VAR_UINT8, direction);
}

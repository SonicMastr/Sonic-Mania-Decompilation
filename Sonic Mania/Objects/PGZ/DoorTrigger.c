#include "SonicMania.h"

ObjectDoorTrigger *DoorTrigger;

void DoorTrigger_Update(void)
{
    RSDK_THIS(DoorTrigger);
    RSDK.ProcessAnimation(&self->animator1);
    if (self->animator2.frameID) {
        if (self->id-- == 1) {
            self->id = RSDK.Rand(15, 121);
            int32 anim   = 0;
            switch (self->orientation) {
                case 0:
                case 1: anim = 3; break;
                case 2:
                case 3: anim = 4; break;
                default: break;
            }

            EntityDebris *shard =
                CREATE_ENTITY(Debris, NULL, (RSDK.Rand(-4, 5) << 16) + self->position.x, (RSDK.Rand(-4, 5) << 16) + self->position.y);
            shard->state     = Debris_State_Move;
            shard->drawFX    = FX_FLIP;
            shard->direction = self->direction;
            shard->drawOrder = Zone->drawOrderHigh - 1;
            RSDK.SetSpriteAnimation(DoorTrigger->aniFrames, anim, &shard->animator, true, 0);
        }
    }
    else {
        foreach_active(Player, player)
        {

            int32 anim    = player->animator.animationID;
            bool32 flag = anim == ANI_JUMP;
            switch (player->characterID) {
                case ID_SONIC: flag |= anim == ANI_DROPDASH; break;
#if RETRO_USE_PLUS
                case ID_MIGHTY: flag |= anim == ANI_DROPDASH; break;
#endif
                case ID_TAILS:
                    if (anim != ANI_JUMP) {
                        flag = anim == ANI_FLY || anim == ANI_FLYTIRED || anim == ANI_FLYLIFT;
                        if (player->position.y <= self->position.y)
                            flag = false;
                    }
                    break;
                case ID_KNUCKLES: flag |= anim == ANI_FLY || anim == ANI_FLYLIFTTIRED; break;
                default: break;
            }

            if (player->sidekick)
                flag = false;

            if (!flag) {
                Player_CheckCollisionBox(player, self, &DoorTrigger->hitboxes[self->animator1.frameID]);
            }
            else {
                if (Player_CheckCollisionTouch(player, self, &DoorTrigger->hitboxes[self->animator1.frameID])) {
                    self->animator2.frameID = 1;
                    if (player->characterID == ID_KNUCKLES && player->animator.animationID == ANI_FLY) {
                        player->velocity.x = -player->velocity.x >> 1;
                        RSDK.SetSpriteAnimation(player->aniFrames, 49, &player->animator, false, 0);
                        player->state = Player_State_KnuxGlideDrop;
                    }
                    else {
                        int32 x = 0, y = 0;
                        if (self->animator1.frameID) {
                            x = player->position.x - self->position.x;
                            if (self->direction)
                                y = (player->position.y - self->position.y) - 0xE0000;
                            else
                                y = (player->position.y - self->position.y) + 0xE0000;
                        }
                        else {
                            if (self->direction)
                                x = (player->position.x - self->position.x) - 0xE0000;
                            else
                                x = (player->position.x - self->position.x) + 0xE0000;
                            y = player->position.y - self->position.y;
                        }

                        int32 angle = RSDK.ATan2(x, y);
#if RETRO_USE_PLUS
                        if (player->characterID == ID_MIGHTY && player->animator.animationID == ANI_DROPDASH) {
                            player->velocity.y -= 0x10000;
                        }
                        else {
#endif
                            player->velocity.x = 0x300 * RSDK.Cos256(angle);
                            player->groundVel  = 0x300 * RSDK.Cos256(angle);
                            player->velocity.y = 0x300 * RSDK.Sin256(angle);
#if RETRO_USE_PLUS
                        }
#endif
                        player->jumpAbility = 0;
                        player->onGround    = false;
                    }
                    RSDK.PlaySfx(DoorTrigger->sfxShatter, false, 255);

                    int32 spawnX = self->position.x;
                    int32 spawnY = self->position.y;
                    switch (self->orientation) {
                        case 0: spawnX -= 0x100000; break;
                        case 1: spawnX += 0x100000; break;
                        case 2: spawnY -= 0x100000; break;
                        case 3: spawnY += 0x100000; break;
                        default: break;
                    }
                    for (int32 i = 0; i < 8; ++i) {
                        EntityDebris *shard =
                            CREATE_ENTITY(Debris, NULL, spawnX + RSDK.Rand(-0xA0000, 0xA0000), spawnY + RSDK.Rand(-0xA0000, 0xA0000));
                        shard->state      = Debris_State_Fall;
                        shard->gravity    = 0x4000;
                        shard->velocity.x = RSDK.Rand(0, 0x20000);
                        if (shard->position.x < self->position.x)
                            shard->velocity.x = -shard->velocity.x;
                        shard->velocity.y = RSDK.Rand(-0x40000, -0x10000);
                        shard->drawFX     = FX_FLIP;
                        shard->direction  = i & 3;
                        shard->drawOrder  = Zone->drawOrderHigh;
                        RSDK.SetSpriteAnimation(ItemBox->aniFrames, 6, &shard->animator, true, RSDK.Rand(0, 4));
                    }
                    RSDK.CreateEntity(Explosion->objectID, intToVoid(1), spawnX, spawnY)->drawOrder = Zone->drawOrderHigh;
                }
            }
        }
    }
}

void DoorTrigger_LateUpdate(void) {}

void DoorTrigger_StaticUpdate(void) {}

void DoorTrigger_Draw(void)
{
    RSDK_THIS(DoorTrigger);
    RSDK.DrawSprite(&self->animator1, NULL, false);
    RSDK.DrawSprite(&self->animator2, NULL, false);
    if (!self->animator2.frameID && !(Zone->timer & 1)) {
        self->inkEffect         = INK_ADD;
        self->animator2.frameID = 2;
        self->alpha             = ((56 * RSDK.Sin256(Zone->timer)) >> 8) + 184;
        RSDK.DrawSprite(&self->animator2, NULL, false);

        self->animator2.frameID = 3;
        RSDK.DrawSprite(&self->animator2, NULL, false);

        self->animator2.frameID = 0;
        self->inkEffect         = INK_NONE;
    }
}

void DoorTrigger_Create(void *data)
{
    RSDK_THIS(DoorTrigger);
    
    self->drawFX = FX_FLIP;
    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_BOUNDS;
        self->visible       = true;
        self->drawOrder     = Zone->drawOrderHigh;
        self->updateRange.y = 0x800000;
        self->updateRange.x = 0x800000;
        RSDK.SetSpriteAnimation(DoorTrigger->aniFrames, 0, &self->animator1, true, 0);

        self->direction = self->orientation & 1;
        if (self->orientation < 2) {
            RSDK.SetSpriteAnimation(DoorTrigger->aniFrames, 1, &self->animator2, true, 0);
        }
        else {
            self->animator1.frameID = 1;
            self->direction *= FLIP_Y;
            RSDK.SetSpriteAnimation(DoorTrigger->aniFrames, 2, &self->animator2, true, 0);
        }
    }
}

void DoorTrigger_StageLoad(void)
{
    if (RSDK.CheckStageFolder("PSZ1"))
        DoorTrigger->aniFrames = RSDK.LoadSpriteAnimation("PSZ1/DoorTrigger.bin", SCOPE_STAGE);
    DoorTrigger->hitboxes[0].top    = -10;
    DoorTrigger->hitboxes[0].left   = -26;
    DoorTrigger->hitboxes[0].bottom = 10;
    DoorTrigger->hitboxes[0].right  = 0;
    DoorTrigger->hitboxes[1].top    = -26;
    DoorTrigger->hitboxes[1].left   = -10;
    DoorTrigger->hitboxes[1].bottom = 0;
    DoorTrigger->hitboxes[1].right  = 10;
    DoorTrigger->sfxShatter         = RSDK.GetSFX("Stage/WindowShatter.wav");
}

#if RETRO_INCLUDE_EDITOR
void DoorTrigger_EditorDraw(void) {}

void DoorTrigger_EditorLoad(void) {}
#endif

void DoorTrigger_Serialize(void)
{
    RSDK_EDITABLE_VAR(DoorTrigger, VAR_UINT8, orientation);
    RSDK_EDITABLE_VAR(DoorTrigger, VAR_UINT8, id);
}

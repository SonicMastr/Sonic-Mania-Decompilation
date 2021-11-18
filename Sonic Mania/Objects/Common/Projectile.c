#include "SonicMania.h"

ObjectProjectile *Projectile;

void Projectile_Update(void)
{
    RSDK_THIS(Projectile);
    StateMachine_Run(self->state);
    if (self->drawFX & FX_ROTATE)
        self->rotation = (self->rotation + self->rotationSpeed) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator);
}

void Projectile_LateUpdate(void) {}

void Projectile_StaticUpdate(void) {}

void Projectile_Draw(void)
{
    RSDK_THIS(Projectile);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Projectile_Create(void *data)
{
    RSDK_THIS(Projectile);
    self->active  = ACTIVE_NORMAL;
    self->visible = true;
    self->state   = (Type_StateMachine)data;
}

void Projectile_StageLoad(void) {}

void Projectile_CheckPlayerCollisions(void)
{
    RSDK_THIS(Projectile);

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &self->hitbox)) {
            switch (self->type) {
                case 0: break;
                case 1:
                    if (self->isProjectile)
                        Player_CheckProjectileHit(player, self);
                    else
                        Player_CheckElementalHit(player, self, SHIELD_FIRE);
                    break;
                case 2:
                    if (self->isProjectile)
                        Player_CheckProjectileHit(player, self);
                    else
                        Player_CheckElementalHit(player, self, SHIELD_LIGHTNING);
                    break;
                case 3: break;
                case 5: break;
                case 6: break;
                case 4:
                case 7: {
                    int32 anim = player->animator.animationID;
                    if (self->isProjectile
#if RETRO_USE_PLUS
                        || (player->characterID == ID_MIGHTY
                            && (anim == ANI_CROUCH || anim == ANI_JUMP || anim == ANI_SPINDASH || anim == ANI_DROPDASH))
#endif
                    ) {
                        if (Player_CheckProjectileHit(player, self)) {
                            self->gravityStrength = 0x3800;
                            self->state           = Projectile_State_MoveGravity;
                        }
                    }
                    else {
                        Player_CheckHit(player, self);
                    }
                    break;
                }
                default: break;
            }
        }
    }
}

void Projectile_State_Move(void)
{
    RSDK_THIS(Projectile);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    if (self->hurtDelay <= 0)
        Projectile_CheckPlayerCollisions();
    else
        self->hurtDelay--;

    if (self->timer <= 0) {
        if (!RSDK.CheckOnScreen(self, NULL))
            destroyEntity(self);
    }
    else {
        if (!self->timer)
            destroyEntity(self);
    }
}

void Projectile_State_MoveGravity(void)
{
    RSDK_THIS(Projectile);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->velocity.y += self->gravityStrength;
    if (self->hurtDelay <= 0)
        Projectile_CheckPlayerCollisions();
    else
        self->hurtDelay--;

    if (self->timer <= 0) {
        if (!RSDK.CheckOnScreen(self, NULL))
            destroyEntity(self);
    }
    else {
        if (!self->timer)
            destroyEntity(self);
    }
}

#if RETRO_INCLUDE_EDITOR
void Projectile_EditorDraw(void) {}

void Projectile_EditorLoad(void) {}
#endif

void Projectile_Serialize(void) {}

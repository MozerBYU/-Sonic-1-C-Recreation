#include "Player.h"
#include "entities/Player.h"
#include "player-sensor.hpp"

#include "player-state-machine.hpp"
#include "player-state-normal.hpp"
#include "player-state-jump.hpp"

// === public === //
void Player::create() 
{
	type = TYPE_PLAYER;
	hitBoxSize = v2f(20, 40);
	anim.create(TEX_OBJECTS);
    anim.set(0, 0, 0);
    standOn = nullptr;

    m_stateMachine.add(new PlayerStateNormal(m_props));
    m_stateMachine.add(new PlayerStateJump(m_props, 6.5));
    m_stateMachine.add(new PlayerStateRoll(m_props));
    m_stateMachine.add(new PlayerStateSkid(m_props));
    m_stateMachine.add(new PlayerStateSpindashCD(m_props));
    m_stateMachine.add(new PlayerStateSpring(m_props));
    m_stateMachine.add(new PlayerStateHurt(m_props));
    m_stateMachine.add(new PlayerStateDie(m_props));

    m_collider.onLanding([this](HexAngle hexAngle){
        m_stateMachine.pushLanding();
    });
}

void Player::update() 
{
    m_stateMachine.update();

    m_collider.setRadius(
        m_stateMachine.isCurling() 
            ? v2i(7, 14) 
            : v2i(9, 19)
    );


    movement();
    if (!debug) m_collider.update();
    angle = m_collider.getAngle().degrees();

    gameplay();
    animation();
    anim.tick();
}

void Player::draw(Camera& cam) 
{
    // Animation angle
    if (animAngle == 360)
        animAngle = 0;

    if (invicTimer == 0 || (invicTimer > 0 && invicTimer % 8 >= 4) || m_stateMachine.currentId() == PlayerStateID::HURT)
        cam.draw(anim, pos, anim8Angle, animFlip, false);

    char dbInfo[128];
    snprintf(
        dbInfo, 127,
        "position: %4.2f %4.2f\n"
        "spd:      %2.2f %2.2f\n"
        "angle:    %X(%4f)\n"
        "flr mode: %d\n"
        "action:   %d\n"
        "ground    %d\n"
        "debug     %d\n",
        pos.x, pos.y,
        spd.x, spd.y,
        m_collider.getAngle().hex, m_collider.getAngle().degrees(),
        m_collider.getMode(),
        m_stateMachine.currentId(),
        m_collider.isGrounded(),
        debug
    );

    Screen& scr = cam.getScr();
    scr.drawText(0, dbInfo, v2f(scr.getSize().width - 256, 8));

    m_collider.draw(cam);
}

void Player::moveCam(Camera& cam)
{
    if (m_stateMachine.currentId() == PlayerStateID::DIE)
        return;

    if (camLagTimer > 0) {
        camLagTimer--; 
        return;
    }

    Size scrSize = cam.getSize();

	float _x = pos.x - (float)scrSize.width / 2;
	float _y = (pos.y - shiftY) - (float)scrSize.height / 2;

	v2f camPos = cam.getPos();

	float _leftBorder = camPos.x - 16;
	float _rightBorder = camPos.x;
	float _topBorder = camPos.y - 32;
	float _bottomBorder = camPos.y + 32;

    if (!endLv) {
        if (_x < _leftBorder)
            camPos.x -= fmin(_leftBorder - _x, 16.0);
        else if (_x > _rightBorder)
            camPos.x += fmin(_x - _rightBorder, 16.0);
    }


	if (m_collider.isGrounded()) {
		if (fabs(spd.y) > 6.0)
			camPos.y += fmin(fmax(_y - camPos.y, -16.0), 16.0);
		else
			camPos.y += fmin(fmax(_y - camPos.y, -6.0), 6.0);
	}
	else {
		if (_y < _topBorder)
			camPos.y -= fmin(_topBorder - _y, 16.0);
		else if (_y > _bottomBorder)
			camPos.y += fmin(_y - _bottomBorder, 16.0);
	}

	//cam.setPos(camPos);
    cam.setPos(v2f(camPos.x, camPos.y));
}

void Player::terrainCollision(Camera& cam) 
{
    if (debug) 
        return;

    if (pos.y + 20 > cam.getBottomBorder() && !cam.isFree()) {
        m_stateMachine.changeTo(PlayerStateID::DIE);
    }

    if (m_stateMachine.currentId() == PlayerStateID::DIE) {
        if (pos.y + 20 > cam.getBottomBorder()+40)
            dead = true;

        return;
    }

    if (endLv) {
        if (pos.x < cam.getPos().x && spd.x < 0) {
            pos.x = cam.getPos().x;
            gsp = 0;
            spd.x = 0;
        } else if (pos.x > cam.getPos().x+cam.getSize().width && spd.x > 0) {
            pos.x = cam.getPos().x+cam.getSize().width;
            gsp = 0;
            spd.x = 0;
        }
    }

    // ===== Gravity ===== //
    if (!m_collider.isGrounded()) {
		angle = 0.0;

        // Air drug
        if (!m_collider.isGrounded() && spd.y < 0.0 && spd.y > -4.0)
            spd.x -= (int(spd.x) / 0.125) / 256;

		if (spd.y < 16.0) {
            // TODO
            if (m_stateMachine.currentId() != PlayerStateID::HURT)
			    spd.y += PL_GRAV;
            else 
                spd.y += 0.21875;
        }
    }

    // === Fall if doesnt have enough speed ===
    // if (m_collider.isGrounded() && (fabs(gsp) < 2.5) && (flrMode != FlrMode::FLOOR)) {
    //     if (angle >= 90.0 && angle <= 270.0) {
    //         flrMode = FlrMode::FLOOR;
    //         m_collider.isGrounded() = false;
    //         angle = 0;
    //         isFalling = true;
    //         gsp = 0.0;
    //     }
    //     horizontalLockTimer = 30;
    // }

    // TODO Wha?
    if (!m_stateMachine.isCurling())
        enemyCombo = 0;
}

// TODO Make better system
void Player::entitiesCollision(std::list<Entity*>& entities, Camera& cam)
{
    if (debug)
        return;

    if (m_stateMachine.currentId() == PlayerStateID::DIE)
        return;

	std::list<Entity*>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
        if (!(*it)->isInCamera(cam)) continue;
        // Collsion Sign Post
		if ((*it)->getType() == TYPE_SIGN_POST) {
			if (pos.x >= (*it)->getPos().x && !endLv) {
                cam.setRightBorder((*it)->getPos().x + (float)cam.getSize().width / 2);
                cam.setBottomBorder((*it)->getPos().y + (float)cam.getSize().height / 2);

                audio.playSound(SND_END_TABLE);

                endLv = true;
            } else if (endLv) {
                SignPost* sp = (SignPost*)*it;
                sp->setAnim(true);
            }
		}
		// Collsion Spring
		if ((*it)->getType() == TYPE_SPRING) {
			Spring* spring = (Spring*)*it;
            switch(spring->getRotation()) {
                case Spring::R_UP:
                    if (!collisionBottom(**it, 2)) break;

                    m_stateMachine.changeTo(PlayerStateID::SPRING);


                    spd.y = -(spring->isRed() ? 16 : 10);
                    spring->doAnim();
                    audio.playSound(SND_SPRING);
                    break;
                case Spring::R_DOWN:
                    if (!collisionTop(**it, 2)) break;
                    
                    m_stateMachine.changeTo(PlayerStateID::SPRING);

                    spd.y = +(spring->isRed() ? 16 : 10);
                    spring->doAnim();
                    audio.playSound(SND_SPRING);
                    break;
                case Spring::R_LEFT:
                    if (!collisionRight(**it) || spd.x <= 0) break;
                    spd.x = -(spring->isRed() ? 16 : 10);
                    gsp = -(spring->isRed() ? 16 : 10);
                    spring->doAnim();
                    audio.playSound(SND_SPRING);
                    break;
                case Spring::R_RIGHT:
                    if (!collisionLeft(**it) || spd.x >= 0) break;
                    spd.x = (spring->isRed() ? 16 : 10);
                    gsp = (spring->isRed() ? 16 : 10);
                    spring->doAnim();
                    audio.playSound(SND_SPRING);
                    break;
			}
		}
		// Collision Ring
		if ((*it)->getType() == TYPE_RING) {
			Ring* ring = (Ring*)*it;
			if (collisionMain(*ring) && ringTimer == 0) {
				ring->destroy();
                AnimMgr rSfx;
                rSfx.create(TEX_OBJECTS);
                rSfx.set(84, 87, 0.5);
                rings++;
                entities.push_back(new Sfx(v2f(ring->getPos().x, ring->getPos().y), rSfx));

                audio.playSound(SND_RING);
			}
		}

        if ((*it)->getType() == TYPE_SPIKES) {
            if (collisionBottom(**it, 2)) {
                getHit(entities);
            }
        }

        // Collision Enemy
		if ((*it)->getType() == TYPE_ENEMY) {
			Enemy* en = (Enemy*)*it;
			if (collisionMain(*en)) {
				if (m_stateMachine.isCurling()) {
                    if (!m_collider.isGrounded()) {
                        if (spd.y > 0) {
                            spd.y *= -1;
                        } else {
                            spd.y -= sign(spd.y);
                        }
                    } 
                    en->destroy();

                    AnimMgr rSfx;
                    rSfx.create(TEX_OBJECTS);
                    rSfx.set(95, 99, 0.125);
                    entities.push_back(new Sfx(v2f(en->getPos().x, en->getPos().y), rSfx));

                    enemyCombo++;
                    switch(enemyCombo) {
                        case 1:  score += 100;  break;
                        case 2:  score += 200;  break;
                        case 3:  score += 500;  break;
                        default: score += 1000; break;
                    }
                    entities.push_back(new EnemyScore(en->getPos(), (EnemyScore::Points)(enemyCombo-1)));
                
                    audio.playSound(SND_DESTROY);
                } else {
                    getHit(entities);
                }
			}
		}
        // Monitor
        if ((*it)->getType() == TYPE_MONITOR) {
            Monitor* m = (Monitor*)*it;
			if (collisionMain(*m) && spd.y >= 0 && m_stateMachine.isCurling()) {
                if (!m_collider.isGrounded()) {
                    if (spd.y > 0) {
                        spd.y *= -1;
                    } else {
                        spd.y -= sign(spd.y);
                    }
                }

                switch (m->getItem()) {
                    case Monitor::M_RINGS:
                        audio.playSound(SND_RING);
                        rings += 10;
                        break;
                    case Monitor::M_SHIELD:
                        audio.playSound(SND_SHIELD); 
                        break;
                    case Monitor::M_INVINCIBILITY: break;
                    case Monitor::M_SPEED: break;
                    case Monitor::M_LIVE: break;                 
                }

                entities.push_back(new BrokenMonitor(m->getPos()));

                AnimMgr rSfx;
                rSfx.create(TEX_OBJECTS);
                rSfx.set(95, 99, 0.125);
                entities.push_back(new Sfx(m->getPos(), rSfx));

                entities.push_back(new MonitorIcon(m->getPos(), m->getItem()));

                m->destroy();

                audio.playSound(SND_DESTROY);
            } 
		}

        // Bullet
        if ((*it)->getType() == TYPE_BULLET) {
			if (collisionMain(**it)) 
                getHit(entities);
		}
        // Collision Layer Switch
		if ((*it)->getType() == TYPE_LAYER_SWITCH) {
			LayerSwitcher* ls = (LayerSwitcher*)*it;
			if (collisionMain(*ls) && m_collider.isGrounded()) {
				if (ls->getMode() == 0 && spd.x > 0) {
                    m_collider.setLayer(terrain::TerrainLayer::NORMAL);
                } else if (ls->getMode() == 1 && spd.x < 0) {
                    m_collider.setLayer(terrain::TerrainLayer::ALTERNATE);
                } else if (ls->getMode() == 2 && spd.x > 0) {
                    m_collider.setLayer(terrain::TerrainLayer::NORMAL);
                } else if (ls->getMode() == 2 && spd.x < 0) {
                    m_collider.setLayer(terrain::TerrainLayer::ALTERNATE);
                }
			}
		}
        //STube
        if ((*it)->getType() == TYPE_STUBE_CNTRL) {
			GimGHZ_STube* _stube = (GimGHZ_STube*)*it;
			if (collisionMain(*_stube)) {
				if (_stube->getMode() == 0) {
                    if (gsp >= 0 || spd.x >= 0) 
                        sTube = true;
                    else if (gsp < 0 && spd.x < 0)
                        sTube = false;
                } else {
                    if (gsp >= 0 || spd.x >= 0) 
                        sTube = false;
                    else if (gsp < 0 && spd.x < 0)
                        sTube = true;
                }

                if (m_stateMachine.currentId() != PlayerStateID::ROLL)
                    audio.playSound(SND_ROLL);
			}
		}
        // GHZ Bridge
        if ((*it)->getType() == TYPE_BRIDGE) {
			GimGHZ_Bridge* br = (GimGHZ_Bridge*)*it;
            if ((!m_collider.isGrounded() && collisionBottom(**it)) || (m_collider.isGrounded() && collisionBottomPlatform(**it, 14))) {
                br->setActive(true);
                pos.y = ((*br).getY()) - ((*br).getHitBoxSize().y / 2) - (hitBoxSize.y / 2);

                if (!standOnObj) {
                    standOn = *it;
                    standOnObj = true;
                    gsp = spd.x;
                }
            }
        }
        // GHZ Slope Platform
        if ((*it)->getType() == TYPE_GHZ_SLP_PLATFORM) {
			GimGHZ_SlpPlatform* slope = (GimGHZ_SlpPlatform*)*it;
            if (((collisionBottom(*slope, 12) && m_collider.isGrounded()) ||
                 (collisionBottom(*slope, 1)  && !m_collider.isGrounded())) && spd.y >= 0 &&
                 pos.y < slope->getPos().y - 30) {
                    int _x = pos.x - (slope->getPos().x - (slope->getHitBoxSize().x / 2));
                    pos.y = slope->getPos().y - (slope->getHeight(_x)) - 22 - hitBoxSize.y / 2;
                    if (!standOnObj) {
                        standOnObj = true;
                        standOn = *it;
                        gsp = spd.x;
                    } else {
                        slope->destroy();
                        
                        if (!slope->isLiving()) {
                            standOnObj = false;
                        }
                    }

                    if (!slope->isLiving())
                        audio.playSound(SND_PLT_CRUSH);
            }
        }
        // Collision Solid
        if ((*it)->isSolid() && (*it)->isLiving()) {
			if (collisionRight(**it) && spd.x > 0.0) {
                pos.x = (*it)->getPos().x - (*it)->getHitBoxSize().x / 2 - hitBoxSize.x / 2;
                spd.x = 0.0;
                gsp = 0.0;
            } 
            if (collisionLeft(**it) && spd.x < 0.0) {
                pos.x = (*it)->getPos().x + (*it)->getHitBoxSize().x / 2 + hitBoxSize.x / 2;
                spd.x = 0.0;
                gsp = 0.0;
            } 

            if (collisionTop(**it) && spd.y < 0.0) {
				pos.y = (*it)->getPos().y + (*it)->getHitBoxSize().y / 2 + hitBoxSize.y / 2;
				spd.y = 0.0;
				gsp = 0.0;
			}
        }
        // Platform
        if ((*it)->isPlatform() && (*it)->isLiving()) {
			if (((collisionBottomPlatform(**it, 12) && m_collider.isGrounded() && spd.y >= 0) ||
                 (collisionBottomPlatform(**it, 1)  && !m_collider.isGrounded())) && spd.y >= 0) {
                if ((*it)->isPlatPushUp())
                    pos.y = (*it)->getPos().y - (*it)->getHitBoxSize().y / 2 - hitBoxSize.y / 2;
                    
                if (!standOnObj) {
                    standOnObj = true;
                    standOn = *it;

                    //if (action == ACT_ROLL)
				        //action = ACT_NORMAL;
                        
                    gsp = spd.x;
                }

                if ((*it)->getType() == TYPE_PLATFORM) {
                    GimGHZ_Platform* plt = (GimGHZ_Platform*)*it;

                    if (plt->isCanFall())
                        plt->setFalling(true);
                    if (plt->getDir() < 2)
                        pos.x += plt->getSpd();
                }
			} 
        }
	}

    if (standOn) {
        if (standOn->isLiving()) {
            if (!collisionBottomPlatform(*standOn, 12) || spd.y < 0) {
                standOnObj = false;
                standOn = nullptr;
            }
        } else {
            standOn = nullptr;
        }
    }
}

// === private === //
void Player::movement() {
    if (m_stateMachine.currentId() == PlayerStateID::DIE)
        return;

    if (pos.x - 9 < 0 && spd.x < 0) {pos.x = 9; gsp = 0; spd.x = 0;}

    if (m_collider.isGrounded()) {
		if ((m_collider.getMode() == PlayerSensorMode::FLOOR && gsp != 0) || (m_collider.getMode() != PlayerSensorMode::FLOOR)) {
			if (m_stateMachine.currentId() != PlayerStateID::ROLL) {
				gsp -= PL_SLP * sinf(radians(angle));
			} else {
				if (sign(gsp) == sign(sinf(radians(angle))))
					gsp -= PL_SLP_ROLL_UP * sinf(radians(angle));
				else
					gsp -= PL_SLP_ROLL_DOWN * sinf(radians(angle));
			}
		}
        spd.x = gsp * cosf(radians(angle));
        spd.y = gsp * -sinf(radians(angle)); 
    }

    pos.x += spd.x;
    pos.y += spd.y;

    if (input.isKeyDebug() && !isDebugPressed) {
        debug = !debug;
        isDebugPressed = true;
    } else if (!input.isKeyDebug()) {
        isDebugPressed = false;
    }


    if (debug) {

        if (input.isKeyLeft())       spd.x -= PL_AIR;
        else if (input.isKeyRight()) spd.x += PL_AIR;
        else spd.x = 0;

        if (input.isKeyUp())         spd.y -= PL_AIR;
        else if (input.isKeyDown())  spd.y += PL_AIR;
        else spd.y = 0;

        return;
    }

    if (m_collider.isGrounded()) {
        if ( m_stateMachine.currentId() != PlayerStateID::ROLL ) {
            if (input.isKeyRight() && !input.isKeyLeft() && canHorMove) {
                if (gsp < 0.0)
                    gsp += PL_DEC;
                else if (gsp < PL_TOP)
                    gsp += PL_ACC;
            } else if (input.isKeyLeft() && !input.isKeyRight() && canHorMove) {
                if (gsp > 0.0)
                    gsp -= PL_DEC;
                else if (gsp > -PL_TOP)
                    gsp -= PL_ACC;
            } else {
                gsp -= fmin(fabs(gsp), PL_FRC) * float(fsign(gsp));
            }
        } else {
            if (input.isKeyRight() && !input.isKeyLeft() && canHorMove) {
                if (gsp < 0.0)
                    gsp += PL_DEC_ROLL;
            } else if (input.isKeyLeft() && !input.isKeyRight() && canHorMove) {
                if (gsp > 0.0)
                    gsp -= PL_DEC_ROLL;
            } 
            gsp -= fmin(fabs(gsp), PL_FRC_ROLL) * float(fsign(gsp));
        }
    } else {
        if ((input.isKeyRight()) && (spd.x < PL_TOP) && canHorMove)
            spd.x += PL_AIR;
        else if ((input.isKeyLeft()) && (spd.x > -PL_TOP) && canHorMove)
            spd.x -= PL_AIR;
    }

}

void Player::gameplay() {
    if (debug)
        return;

    if (m_stateMachine.currentId() == PlayerStateID::DIE)
        return;

    // invincibility Timer
    if (invicTimer > 0 && m_stateMachine.currentId() != PlayerStateID::HURT)
        invicTimer--;
    if (ringTimer > 0)
        ringTimer--;

    // Hor lock timer
    if (horizontalLockTimer > -1) {
        // if (horizontalLockTimer == 0)
        //     canHorMove = true;
        // else
        //     canHorMove = false;

        horizontalLockTimer--;
    }

    // S Tube
    if (sTube) {
        m_stateMachine.changeTo(PlayerStateID::ROLL);

        if (gsp == 0.0) {
            gsp = (!animFlip) ? 2.0 : -2.0;
        }
    }

    // === Jump ===
    if (input.isKeyAction() && m_collider.isGrounded() && !sTube && m_collider.isPlayerCanJump()) {
        m_stateMachine.changeTo(PlayerStateID::JUMP);
    }

    // Shift y pos and change m_collider.isGrounded() sensor width when player is rolling 
    if (m_stateMachine.isCurling()) {
		hitBoxSize = v2f(20, 30);
        shiftY = 5;
    } else {
		hitBoxSize = v2f(20, 40);
        shiftY = 0;
    }

}

void Player::animation() {
    //Animation angle
    if (m_collider.isGrounded()) {
		if (!m_stateMachine.isCurling() && (abs(gsp) > 0))
            animAngle = angle;
		else
			animAngle = 0.0;
	} else {
        if (animAngle != 0 && m_stateMachine.currentId() != PlayerStateID::JUMP) {
			if (animAngle > 180.0)
				animAngle += 4.0;
			else
				animAngle -= 4.0;

			if (animAngle > 360.0)
				animAngle = 0.0;
			else if (animAngle < 0.0)
				animAngle = 0.0;
        } else {
            animAngle = 0;
        }
    }

    //Transform Angle
    diaAnim = false;
    if (animAngle > 337.5 || animAngle <= 22.5) {
        anim8Angle = 0;
    } else if (animAngle > 22.5 && animAngle <= 67.5) {
        if (!animFlip) anim8Angle = 0;
        else anim8Angle = 270;
        diaAnim = true;
    } else if (animAngle > 67.5 && animAngle <= 112.5) {
        anim8Angle = 270;
    } else if (animAngle > 112.5 && animAngle <= 157.5) {
        if (!animFlip) anim8Angle = 270;
        else anim8Angle = 180;
        diaAnim = true;
    } else if (animAngle > 157.5 && animAngle <= 202.5) {
        anim8Angle = 180;
    } else if (animAngle > 202.5 && animAngle <= 247.5) {
        if (!animFlip) anim8Angle = 180;
        else anim8Angle = 90;
        diaAnim = true;
    } else if (animAngle > 247.5 && animAngle <= 292.5) {
        anim8Angle = 90;
    } else if (animAngle > 292.5 && animAngle <= 337.5) {
        if (!animFlip) anim8Angle = 90;
        else anim8Angle = 0;
        diaAnim = true;
    } 

    // Set sprite direction
    if (m_collider.isGrounded() && m_stateMachine.currentId() != PlayerStateID::ROLL) {
        if ((gsp < 0.0) && (input.isKeyLeft()))
            animFlip = true;
        else if ((gsp > 0.0) && (input.isKeyRight()))
            animFlip = false;
    } else {
        if (input.isKeyLeft())
            animFlip = true;
        else if (input.isKeyRight())
            animFlip = false;
    }

}

void Player::getHit(std::list<Entity*>& entities) {
    if (m_stateMachine.currentId() == PlayerStateID::HURT || invicTimer > 0) return;

    if (rings != 0) {
        ringTimer = 64;
        m_collider.forceToAir();
        invicTimer = 120;
        
        m_stateMachine.changeTo(PlayerStateID::HURT);
       
        //Rings
        int t = 0;
        float _angle = 101.25f;
        bool _n = false;
        float _spd = 4.0;
        int _h = 1;
        while(t < rings && t < 32) {
            // TODO
            // Ring* r = new Ring(pos, *trn, float(cosf(radians(_angle)) * _spd * _h), 
            //     float(-sinf(radians(_angle)) * _spd));
            // r->create();
            // entities.push_back(r);
            _h *= -1;
            if (_n)
                _angle += 22.5f;
            _n = !_n;
            t++;
            if (t == 16) {
                _spd = 2;
                _angle = 101.25;
            }
        } 
        rings = 0;

        audio.playSound(SND_RING_LOSS);
    } else {
        m_stateMachine.changeTo(PlayerStateID::DIE);
    }
}

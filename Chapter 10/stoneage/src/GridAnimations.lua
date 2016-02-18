local constants = require ("constants")

local GridAnimations = class("GridAnimations")

function GridAnimations.create ( )
    local ga = GridAnimations.new ()
    return ga
end

function GridAnimations:setGameLayer (value)
    self.gameLayer = value
end

function GridAnimations:ctor()
    self.animatedGems = 0
    self.animatedMatchedGems = 0
    self.gameLayer = nil
    self.animatedCollapsedGems = 0
end


function GridAnimations:animateIntro ()
    self.gameLayer.enabled = false 
    self.animatedGems = 0
    for i = 1, constants.GRID_SIZE_X do 
        self:dropColumn ( i )
    end
end

function GridAnimations:dropColumn (col)

    local function onAnimatedColumn (sender)
        self.animatedGems = self.animatedGems + 1
        if (self.animatedGems == constants.GEMS_TOTAL) then
            self.gameLayer.enabled = true
            self.gameLayer:startTimer()
        end
    end

    local delay = 0
    if (col > 1) then delay = math.random() * 1.5 end
     
    local gem = nil 
    for i = 1, constants.GRID_SIZE_Y do
        gem = self.gameLayer.gridGemsColumnMap[col][i]
        gem:setVisible(true)
        local finalY = gem:getPositionY()
        gem:setPositionY(finalY + 800)
        local moveTo = cc.MoveTo:create(2, cc.p(gem:getPositionX(), finalY  ))
        local move_ease_out = cc.EaseBounceOut:create( moveTo )
        local delayAction = cc.DelayTime:create(delay)
        local callback = cc.CallFunc:create(onAnimatedColumn)
        local seq1 = cc.Sequence:create(delayAction, move_ease_out, callback)
        gem:runAction(seq1)
    end
end

function GridAnimations:animateSelected (gem)
    gem:select()
    gem:stopAllActions()
    local rotate = cc.EaseBounceOut:create ( cc.RotateBy:create(0.5, 360) )
    gem:runAction(rotate)
end

function GridAnimations:resetSelectedGem ()
    local gem = self.gameLayer.selectedGem
    local gemPosition = self.gameLayer.selectedGemPosition
    local moveTo = cc.MoveTo:create(0.25, gemPosition)
    local move_elastic_out = cc.EaseElasticOut:create( moveTo )
    gem:runAction(move_elastic_out)
end

function GridAnimations:swapGems (gemOrigin, gemTarget, onComplete)
    
    gemOrigin:deselect()
    
    local origin = self.gameLayer.selectedGemPosition
    local target = cc.p(gemTarget:getPositionX(), gemTarget:getPositionY()) 
            
    local moveSelected = cc.EaseBackOut:create(cc.MoveTo:create(0.8, target) )   
    local moveTarget = cc.EaseBackOut:create(cc.MoveTo:create(0.8, origin) )
    local callback = cc.CallFunc:create(onComplete)
    
    gemOrigin:runAction(moveSelected)
    gemTarget:runAction (cc.Sequence:create(moveTarget, callback))
end

function GridAnimations:animateMatches (matches, onComplete)
    
    local function onCompleteMe (sender)
        
        self.animatedMatchedGems = self.animatedMatchedGems - 1;
        if (self.animatedMatchedGems == 0) then
            if (onComplete ~= nil) then onComplete() end
        end

    end
    self.animatedMatchedGems = #matches
    
    local gem = nil
    for i, point in ipairs(matches) do
        gem = self.gameLayer.gridGemsColumnMap[point.x][point.y]
        gem:stopAllActions()
        local scale = cc.EaseBackOut:create ( cc.ScaleTo:create(0.3, 0))
        local callback = cc.CallFunc:create(onCompleteMe)
        local action = cc.Sequence:create (scale, callback)
        gem.gemContainer:runAction(action)
    end
end


function GridAnimations:animateCollapse ( onComplete )
    self.animatedCollapsedGems = 0
    local gem = nil
    local drop  = 1
   
    for c = 1, constants.GRID_SIZE_X do 
        drop = 1
        for r = 1, constants.GRID_SIZE_Y do
            gem = self.gameLayer.gridGemsColumnMap[c][r]
            --if this gem has been resized, move it to the top 
            if (gem.gemContainer:getScaleX() ~= 1) then
                gem:setPositionY((constants.GRID_SIZE_Y + (drop)) * (constants.TILE_SIZE + constants.GRID_SPACE))
                self.animatedCollapsedGems = self.animatedCollapsedGems + 1
                gem:setType ( self.gameLayer:getNewGem() )
                gem:setVisible(true)
                local newY = (constants.GRID_SIZE_Y - (drop - 1)) * (constants.TILE_SIZE + constants.GRID_SPACE)
                self:dropGemTo (gem, newY,  0.2, onComplete)
                drop = drop + 1
            else
            
                if (drop > 1) then
                    self.animatedCollapsedGems = self.animatedCollapsedGems + 1
                    local newY = gem:getPositionY() - (drop - 1) * (constants.TILE_SIZE + constants.GRID_SPACE)
                    self:dropGemTo (gem, newY, 0, onComplete)
                end

            end
        end 
    end
end

function GridAnimations:dropGemTo (gem, y, delay, onComplete)
   
    gem:stopAllActions()
    gem:reset()

    local function onCompleteMe  (sender)
        self.animatedCollapsedGems = self.animatedCollapsedGems - 1;
        if ( self.animatedCollapsedGems == 0 ) then
            if (onComplete ~= nil) then onComplete() end
        end
    end

    local move = cc.EaseBounceOut:create (cc.MoveTo:create (0.6, cc.p(gem:getPositionX(), y) ) )
    
    local action = cc.Sequence:create (cc.DelayTime:create(delay), move, cc.CallFunc:create(onCompleteMe))
    gem:runAction(action)
end

function GridAnimations:collectDiamonds(diamonds)
    
    local function removeDiamond (sender)
        sender:setVisible(false)
    end
    
    for i = 1, #diamonds do
        local delay = cc.DelayTime:create(i * 0.05)
        local moveTo = cc.EaseBackIn:create( cc.MoveTo:create( 0.8, cc.p(50, constants.SCREEN_HEIGHT - 50) ) )
        local action = cc.Sequence:create (delay, moveTo, cc.CallFunc:create(removeDiamond))
        diamonds[i]:runAction(action)
    end
end


return GridAnimations
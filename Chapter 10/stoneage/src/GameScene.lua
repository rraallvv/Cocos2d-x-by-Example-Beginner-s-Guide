--[[
Background music: Life of Reilly by
Kevin MacLeod - kevin@incompetech.com
http://incompetech.com/
--]]

local constants = require ("constants")
local GridController = require ("GridController")
local GridAnimations = require ("GridAnimations")
local ObjectPools = require ("ObjectPools")
local Gem = require ("Gem")

local GameScene = class("GameScene",function()
    return cc.Scene:create()
end)

function GameScene.create()
    local scene = GameScene.new()
    return scene
end

function GameScene:ctor()
    self.visibleSize = cc.Director:getInstance():getVisibleSize()
    self.middle = {x = self.visibleSize.width * 0.5, y = self.visibleSize.height * 0.5}
    self.origin = cc.Director:getInstance():getVisibleOrigin()
    self.schedulerID = nil
    self.grid = {}
    self.gridController = nil
    self.gridAnimations = nil
    self.objectPools = nil
    self.gridGemsColumnMap = {}
    self.allGems = {}
    self.gemsContainer = cc.Node:create()
    self.selectedGem = nil
    self.targetGem = nil
    self.selectedIndex = {x = 0, y = 0}
    self.targetIndex = {x = 0, y = 0}
    self.selectedGemPosition = {x = 0, y = 0}
    self.combos = 0
    self.addingCombos = false
    self.scoreLabel = nil
    self.diamondScoreLabel = nil
    self.diamondScore = 0
    self.gemsScore = 0
    self.running = true
    self:addTouchEvents()
    self:init()
    self:buildGrid()
end

function GameScene:addTouchEvents()

    local bg = cc.Sprite:create("background.jpg")
    bg:setPosition(self.middle.x, self.middle.y)
    self:addChild(bg)

    local function onTouchBegan(touch, event)
        self.gridController:onTouchDown(touch:getLocation())
        return true
    end

    local function onTouchMoved(touch, event)
        self.gridController:onTouchMove(touch:getLocation())
    end

    local function onTouchEnded(touch, event)
        self.gridController:onTouchUp(touch:getLocation())
    end

    local listener = cc.EventListenerTouchOneByOne:create()
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = bg:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, bg)
end

function GameScene:init ()
    
    self.gridController = GridController:create()
    self.gridAnimations = GridAnimations:create()
    self.objectPools = ObjectPools.create()
    
    self.gridAnimations:setGameLayer(self)
    self.gridController:setGameLayer(self)
    self.objectPools:createPools(self)
    
    self:addChild( self.gemsContainer )
    self.gemsContainer:setPosition( 25, 80)
    
    --build interface
    local frame = cc.Sprite:create("frame.png")
    frame:setPosition(self.middle.x, self.middle.y)
    self:addChild(frame)
    
    local diamondScoreBg = cc.Sprite:create("diamondScore.png")
    diamondScoreBg:setPosition(100, constants.SCREEN_HEIGHT - 30)
    self:addChild(diamondScoreBg)
    
    local scoreBg = cc.Sprite:create("gemsScore.png")
    scoreBg:setPosition(280, constants.SCREEN_HEIGHT - 30)
    self:addChild(scoreBg)
    
    local ttfConfig = {}
    ttfConfig.fontFilePath="fonts/myriad-pro.ttf"
    ttfConfig.fontSize=20

    self.diamondScoreLabel = cc.Label:createWithTTF(ttfConfig, "0", cc.TEXT_ALIGNMENT_RIGHT , 150)    
    self.diamondScoreLabel:setPosition (140, constants.SCREEN_HEIGHT - 30)
    self:addChild(self.diamondScoreLabel)
    
    self.scoreLabel = cc.Label:createWithTTF(ttfConfig, "0", cc.TEXT_ALIGNMENT_RIGHT , 150)    
    self.scoreLabel:setPosition (330, constants.SCREEN_HEIGHT - 30)
    self:addChild(self.scoreLabel)
 
end


function GameScene:buildGrid ()

    math.randomseed(os.clock())
   
    self.enabled = false
    local g
    for c = 1, constants.GRID_SIZE_X do
        self.grid[c] = {}
        self.gridGemsColumnMap[c] = {}
        for r = 1, constants.GRID_SIZE_Y do
            if (c < 3) then
                self.grid[c][r] = constants.TYPES[ self:getVerticalUnique(c,r) ]
            else
                self.grid[c][r] = constants.TYPES[ self:getVerticalHorizontalUnique(c,r) ]
            end
           
            g = Gem:create()
            g:setType(  self.grid[c][r] )
           
            g:setPosition ( c * (constants.TILE_SIZE + constants.GRID_SPACE), 
                r * (constants.TILE_SIZE + constants.GRID_SPACE))
           
            self.gemsContainer:addChild(g)           
            self.gridGemsColumnMap[c][r] = g
            table.insert(self.allGems, g)
            
        end
    end
    self.gridAnimations:animateIntro()
    
end

function GameScene:getVerticalUnique (col, row)

    local type = math.floor (math.random () * #constants.TYPES + 1 )

    if (self.grid[col][row-1] == constants.TYPES[type] and self.grid[col][row-2] ~= nil and self.grid[col][row-2] == constants.TYPES[type]) then
        type = type + 1; 
        if (type == #constants.TYPES + 1) then type = 1 end
    end
    return type
end

function GameScene:getVerticalHorizontalUnique (col, row)
    
    local type = self:getVerticalUnique (col, row)
    
    if (self.grid[col - 1][row] == constants.TYPES[type] and self.grid[col - 2][row] ~= nil and self.grid[col - 2][row] == constants.TYPES[type]) then
        local unique = false
        while unique == false do
          type = self:getVerticalUnique (col, row)
          if (self.grid[col-1][row] == constants.TYPES[type] and
          self.grid[col - 2 ][row] ~= nil and  self.grid[col - 2 ][row] == constants.TYPES[type]) then
            --do nothing
          else
             unique = true
          end           
        end
    end
    return type
end

function GameScene:swapGemsToNewPosition ()

    local function onMatchedAnimatedOut (sender)
        self:collapseGrid()
    end
    
    local function onReturnSwapComplete (sender)
        self.gridController.enabled = true
    end

    local function onNewSwapComplete (sender)
       
        self.gridGemsColumnMap[self.targetIndex.x][self.targetIndex.y] = self.selectedGem
        self.gridGemsColumnMap[self.selectedIndex.x][self.selectedIndex.y] = self.targetGem

        self.grid[self.targetIndex.x][self.targetIndex.y] = self.selectedGem.type
        self.grid[self.selectedIndex.x][self.selectedIndex.y] = self.targetGem.type

        self.combos = 0
        self.addingCombos = true
        --check for new matches
        if (self.gridController:checkGridMatches() == true) then
            --animate matched gems
            if (#self.gridController.matchArray > 3) then self.combos = self.combos + (#self.gridController.matchArray - 3) end 
            self.gridAnimations:animateMatches (self.gridController.matchArray, onMatchedAnimatedOut)
            self:showMatchParticle (self.gridController.matchArray)
            self:setGemsScore(#self.gridController.matchArray * constants.POINTS)
            self:playFX("match2.wav")
        else
            --no matches, swap gems back
            self.gridAnimations:swapGems (self.targetGem, self.selectedGem, onReturnSwapComplete)

            self.gridGemsColumnMap[self.targetIndex.x][self.targetIndex.y] = self.targetGem
            self.gridGemsColumnMap[self.selectedIndex.x][self.selectedIndex.y] = self.selectedGem

            self.grid[self.targetIndex.x][self.targetIndex.y] = self.targetGem.type
            self.grid[self.selectedIndex.x][self.selectedIndex.y] = self.selectedGem.type
            self:playFX("wrong.wav")
        end

        self.selectedGem = nil
        self.targetGem = nil
    end
    
    self.gridAnimations:swapGems (self.selectedGem, self.targetGem, onNewSwapComplete)
    
end

function GameScene:collapseGrid ()
    
    local function onGridCollapseComplete (sender)
        
        local function onMatchedAnimatedOut (sender)
            self:collapseGrid()
        end
        
        for i = 1, #self.allGems do
            local gem = self.allGems[i]
            local xIndex = math.ceil ((gem:getPositionX() - constants.TILE_SIZE * 0.5) / (constants.TILE_SIZE + constants.GRID_SPACE))
            local yIndex = math.ceil ((gem:getPositionY() - constants.TILE_SIZE * 0.5) / (constants.TILE_SIZE + constants.GRID_SPACE))
            self.gridGemsColumnMap[xIndex][yIndex] = gem
            self.grid[xIndex][yIndex] = gem.type
        end

        if (self.gridController:checkGridMatches () == true) then
           --animate matched games
           if (self.addingCombos == true) then
               if (#self.gridController.matchArray > 3) then self.combos = self.combos + (#self.gridController.matchArray - 3) end
           end
           self.gridAnimations:animateMatches (self.gridController.matchArray, onMatchedAnimatedOut)
           self:showMatchParticle (self.gridController.matchArray)
           self:setGemsScore(#self.gridController.matchArray * constants.POINTS)
           self:playFX("match.wav")
        else 
            --no more matches, check for combos
            if (self.combos > 0) then
                --now turn random gems into diamonds
                local diamonds = {}
                local removeGems = {}
                local i = 0
                
                math.randomseed(os.clock())
                
                while i < self.combos do
                    
                    i = i + 1
                    
                    local randomGem = nil
                    local randomX,randomY = 0
                    
                    while randomGem == nil do
                        randomX = math.random(1, constants.GRID_SIZE_X)
                        randomY = math.random(1, constants.GRID_SIZE_Y)
                        randomGem = self.gridGemsColumnMap[randomX][randomY]
                        if (randomGem.type == constants.TYPE_GEM_WHITE) then randomGem = nil end
                    end
                    
                    local diamond = self.objectPools:getDiamond()
                    diamond:setPosition(randomGem:getPositionX(), randomGem:getPositionY())
                    
                    local diamondParticle = self.objectPools:getDiamondParticle()
                    diamondParticle:setPosition(randomGem:getPositionX(), randomGem:getPositionY())
                    
                    table.insert(diamonds, diamond)
                    table.insert(removeGems, {x=randomX, y=randomY}) 
                                                            
                end
                self:setDiamondScore(#diamonds * constants.DIAMOND_POINTS)
                self.gridAnimations:animateMatches(removeGems, onMatchedAnimatedOut)                
                self.gridAnimations:collectDiamonds(diamonds)
                self.combos = 0 
                self:playFX("diamond.wav")                 
            else
                self.gridController.enabled = true
            end
            self.addingCombos = false
        end
    end
    
    for i = 1, #self.gridController.matchArray do
        self.grid[self.gridController.matchArray[i].x][self.gridController.matchArray[i].y] = -1
    end

    local column = nil
    local newColumn = nil
    local i

    for c = 1, constants.GRID_SIZE_X do
        column = self.grid[c]
        newColumn = {}
        i = 1
        while #newColumn < #column do
            if (#column > i) then
                if (column[i] ~= -1) then
                    --move gem
                    table.insert(newColumn, column[i])
                end
            else
                --create new gem
                table.insert(newColumn, 1, column[i])
            end
            i = i+1            
        end
        self.grid[c] = newColumn
    end
    self.gridAnimations:animateCollapse (onGridCollapseComplete)
end

function GameScene:dropSelectedGem ()
    self.selectedGem:setLocalZOrder(constants.Z_GRID)
    self.gridAnimations:resetSelectedGem()
end

function GameScene:getNewGem ()
    return math.random(1, #constants.TYPES)
end

function GameScene:addToScore ()
    for i = 1, #self.gridController.matchArray do
        local position = self.gridController.matchArray[i]
        local gem = self.gridGemsColumnMap[position.x][position.y]
        if (gem.type == constants.TYPE_GEM_WHITE) then
            --got a diamond
        end
    end
end

function GameScene:showMatchParticle (matches)
    
    for i = 1, #matches do
        local gem = self.gridGemsColumnMap[matches[i].x][matches[i].y]
        local particle = self.objectPools:getMatchParticle()
        particle:setPosition(gem:getPositionX() + self.gemsContainer:getPositionX(), gem:getPositionY()  + self.gemsContainer:getPositionY())       
    end
    
end

function GameScene:setDiamondScore (value)
    self.diamondScore = self.diamondScore  + value
    self.diamondScoreLabel:setString("" .. self.diamondScore) 
end

function GameScene:setGemsScore (value)
    self.gemsScore = self.gemsScore  + value
    self.scoreLabel:setString("" .. self.gemsScore)
end

function GameScene:playFX(name)
    local fx = cc.FileUtils:getInstance():fullPathForFilename(name) 
    cc.SimpleAudioEngine:getInstance():playEffect(fx)
end

function GameScene:startTimer()

    local timeBarBg = cc.Sprite:create("timeBarBg.png")
    timeBarBg:setPosition(self.middle.x, 40)
    self:addChild(timeBarBg)

    local timeBar = cc.Sprite:create("timeBar.png")
    timeBar:setAnchorPoint(cc.p(0,0.5))
    timeBar:setPosition(self.middle.x - 290, 40)
    self:addChild(timeBar)
    
    local function tick()
        local scaleNow = timeBar:getScaleX()
        local speed = 0.007
        if (scaleNow - speed > 0) then
            timeBar:setScaleX(scaleNow - speed)
        else
            --GameOver!!!
            timeBar:setScaleX(0)
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.schedulerID)
            cc.SimpleAudioEngine:getInstance():stopMusic()
            self.running = false            
            -- show game over
            local gameOver = cc.Sprite:create("gameOver.png")
            gameOver:setPosition(self.middle.x, self.middle.y)
            self:addChild(gameOver)
        end 
    end
  
    self.schedulerID = cc.Director:getInstance():getScheduler():scheduleScriptFunc(tick, 1, false)
end

return GameScene

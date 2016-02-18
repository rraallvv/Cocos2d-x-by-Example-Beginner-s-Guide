local constants = require ("constants")

local ObjectPools = class("ObjectPools")

function ObjectPools.create (  )
    local op = ObjectPools.new ()
    return op
end

function ObjectPools:ctor() 
    self.diamondPool = {}
    self.diamondParticlePool = {}
    self.matchParticlePool = {}
    self.diamondIndex = 1
    self.diamondParticleIndex = 1
    self.matchParticleIndex = 1
end

function ObjectPools:getDiamond ()
    local diamond = self.diamondPool[self.diamondIndex]
    diamond:setVisible(true)
    self.diamondIndex = self.diamondIndex + 1
    if ( self.diamondIndex > #self.diamondPool ) then self.diamondIndex = 1 end
    return diamond
end

function ObjectPools:getMatchParticle ()
    local particle = self.matchParticlePool[self.matchParticleIndex]
    particle:setVisible (true)
    particle:resetSystem()
    self.matchParticleIndex = self.matchParticleIndex + 1
    if ( self.matchParticleIndex > #self.matchParticlePool ) then self.matchParticleIndex = 1 end
    return particle
end

function ObjectPools:getDiamondParticle ()
    local particle = self.diamondParticlePool[self.diamondParticleIndex]
    particle:setVisible (true)

    particle:resetSystem()
    self.diamondParticleIndex = self.diamondParticleIndex + 1
    if ( self.diamondParticleIndex > #self.diamondParticlePool ) then self.diamondParticleIndex = 1 end
    return particle
end

function ObjectPools:createPools (gameLayer)

    for i = 1, 50 do
        local diamondParticle = cc.ParticleSystemQuad:create("diamond.plist")
        diamondParticle:stopSystem()
        
        diamondParticle:setVisible(false)
        gameLayer:addChild(diamondParticle)
        table.insert(self.diamondParticlePool, diamondParticle)
    end
    
    for i = 1, 50 do
        local matchParticle = cc.ParticleSystemQuad:create("match.plist")
        matchParticle:stopSystem()
        matchParticle:setVisible(false)
        gameLayer:addChild(matchParticle)
        table.insert(self.matchParticlePool, matchParticle)
    end
    
    for i = 1, 50 do
        local diamond = cc.Sprite:create("gem_white.png")
        diamond:setLocalZOrder(constants.Z_DIAMOND)
        diamond:setVisible(false)
        gameLayer:addChild(diamond)
        table.insert(self.diamondPool, diamond)
    end
end
return ObjectPools
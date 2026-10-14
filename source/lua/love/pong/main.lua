-- PONG game implementation in `lua` language using `love2d` framework
APP_DEBUG = true

function APP_ASSERT( success, message )
    if APP_DEBUG and not success then error( message or "APP_ASSERT failed", 2 ) end
    return success
end

function hex_color( r, g, b )
    APP_ASSERT( 0x00 <= r and r <= 0xFF, "r is out of range" )
    APP_ASSERT( 0x00 <= g and g <= 0xFF, "g is out of range" )
    APP_ASSERT( 0x00 <= b and b <= 0xFF, "b is out of range" )

    return r / 0xFF, g / 0xFF, b / 0xFF
end

function love.draw( )
    love.graphics.setBackgroundColor( hex_color( 0x24, 0x32, 0x28 ))
end

function love.keypressed( key )
    if key == "escape" then love.event.quit( ) end
end

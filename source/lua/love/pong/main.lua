-- PONG game implementation in `lua` language using `love2d` framework
APP_DEBUG = true

APP_QUIT_READY = false

function APP_ASSERT( success, message )
    if APP_DEBUG and not success then error( message or "APP_ASSERT failed", 2 ) end
    return success
end

function APP_PRINT( message )
    if APP_DEBUG then print( message ) end
end

function app_quit( )
    love.event.quit( )
end

function hex_color( r, g, b )
    APP_ASSERT( 0x00 <= r and r <= 0xFF, "r is out of range" )
    APP_ASSERT( 0x00 <= g and g <= 0xFF, "g is out of range" )
    APP_ASSERT( 0x00 <= b and b <= 0xFF, "b is out of range" )

    return r / 0xFF, g / 0xFF, b / 0xFF
end

function love.load( )
    love.window.setMode( 1280, 720 )
end

function love.draw( )
    love.graphics.setBackgroundColor( hex_color( 0x24, 0x32, 0x28 ))
end

function love.keypressed( key )
    APP_PRINT( string.format( "keypressed key: %s", key))

    if key == "escape" then app_quit( ) end
end

function love.joystickhat( joy, hat, dir )
    APP_PRINT( string.format("joystick hat #%d shifted to direction: %s", hat, dir))

    if dir == 'd' then APP_QUIT_READY = true
    else APP_QUIT_READY = false end
end

function love.joystickpressed( joystick, button )
    APP_PRINT( string.format("joystick fired raw button number: %d", button))

    if button == 10 and APP_QUIT_READY then app_quit( ) end
end

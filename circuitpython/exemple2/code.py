import time
import board, displayio, framebufferio, rgbmatrix

displayio.release_displays()

matrix = rgbmatrix.RGBMatrix(
    width=32, bit_depth=4,
    rgb_pins=[board.MTX_R1, board.MTX_G1, board.MTX_B1,
              board.MTX_R2, board.MTX_G2, board.MTX_B2],
    addr_pins=[board.MTX_ADDRA, board.MTX_ADDRB,
               board.MTX_ADDRC, board.MTX_ADDRD],
    clock_pin=board.MTX_CLK, latch_pin=board.MTX_LAT,
    output_enable_pin=board.MTX_OE)

display = framebufferio.FramebufferDisplay(matrix, auto_refresh=True)
display.rotation = 180  # à retirer si l'orientation est déjà bonne

N = 64
bitmap = displayio.Bitmap(32, 32, N)
palette = displayio.Palette(N)
group = displayio.Group()
group.append(displayio.TileGrid(bitmap, pixel_shader=palette))
display.root_group = group

def wheel(p):
    p %= 256
    if p < 85:
        return (255 - p * 3, p * 3, 0)
    if p < 170:
        p -= 85
        return (0, 255 - p * 3, p * 3)
    p -= 170
    return (p * 3, 0, 255 - p * 3)

# Phase 1 : aplats
for couleur in [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 255)]:
    palette[0] = couleur
    bitmap.fill(0)
    time.sleep(2)

# Phase 2 : arc-en-ciel diagonal qui tourne
for y in range(32):
    for x in range(32):
        bitmap[x, y] = (x + y) % N

decalage = 0
while True:
    for i in range(N):
        palette[i] = wheel((i + decalage) * 4)
    decalage = (decalage + 1) % N
    time.sleep(0.03)
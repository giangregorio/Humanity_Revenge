bool checkhitbox(byte x1, byte y1, byte w1, byte h1, byte x2, byte y2, byte w2, byte h2)
{
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

bool checkhitboxcr(byte xc, byte yc, byte rc, byte x1, byte y1, byte w1, byte h1)
{
    byte circleDistanceX = abs(xc - x1);
    byte circleDistanceY = abs(yc - y1);

    if (circleDistanceX > (w1/2 + rc)) { return false; }
    if (circleDistanceY > (h1/2 + rc)) { return false; }

    if (circleDistanceX <= (w1/2)) { return true; } 
    if (circleDistanceY <= (h1/2)) { return true; }

    int cornerDistance_sq = (circleDistanceX - w1/2)^2 + (circleDistanceY - h1/2)^2;

    return (cornerDistance_sq <= (rc^2));
}







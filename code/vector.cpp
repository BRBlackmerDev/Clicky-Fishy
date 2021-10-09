
#ifdef	STD_INCLUDE_DECL

//----------
// vector declarations
//----------

union vec2 {
	struct {
		flo32 x;
		flo32 y;
	};
	flo32 elem[2];
};

union vec3 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
	};
    struct {
        vec2  xy;
        flo32 z;
    };
	flo32 elem[3];
};

union vec4 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
		flo32 w;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
		flo32 a;
	};
	struct {
		vec3 xyz;
		flo32  w;
	};
    struct {
        vec3 rgb;
        flo32  w;
    };
	flo32 elem[4];
};

//----------
// rect declarations
//----------

union rect {
	struct {
		vec2 min;
		vec2 max;
	};
	struct {
		flo32 left;
		flo32 bottom;
		flo32 right;
		flo32 top;
	};
	flo32 elem[4];
};

struct RECT_PERIMETER {
    vec2 position;
    vec2 normal;
};

//----------
// oriented rect declarations
//----------

struct orect {
    vec2 center;
    vec2 halfDim;
    vec2 xAxis;
    vec2 yAxis;
};

//----------
// line segment declarations
//----------

struct LINE_SEG {
    vec2 P;
    vec2 Q;
};

//----------
// circle declarations
//----------

struct circ {
    vec2  center;
    flo32 radius;
};

//----------
// capsule2 declarations
//----------

struct CAPSULE2 {
    vec2  P;
    vec2  Q;
    flo32 radius;
};

//----------
// mat 3x3 declarations
//----------

union mat3 {
	vec3  column[ 3 ];
	flo32 elem[ 9 ];
};

//----------
// mat 4x4 declarations
//----------

union mat4 {
	vec4 column[ 4 ];
	flo32 elem[ 16 ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// vec2 functions
//----------

inline vec2 Vec2( flo32 a, flo32 b ) { vec2 result = { a, b }; return result; }
inline vec2 operator+( vec2 a, vec2 b ) { vec2 result = { a.x + b.x, a.y + b.y }; return result; }
inline vec2 operator-( vec2 a, vec2 b ) { vec2 result = { a.x - b.x, a.y - b.y }; return result; }
inline vec2 operator-( vec2 a ) { vec2 result = { -a.x, -a.y }; return result; }
inline vec2 operator*( vec2 a, flo32 f ) { vec2 result = { a.x * f, a.y * f }; return result; }
inline vec2 operator*( flo32 f, vec2 a ) { vec2 result = a * f; return result; }
inline vec2 operator*( vec2 a, vec2 b ) { vec2 result = { a.x * b.x, a.y * b.y }; return result; }
inline vec2 operator/( vec2 a, flo32 f ) { vec2 result = {}; if( f != 0.0f ) { result = Vec2( a.x / f, a.y / f ); } return result; }
inline vec2 operator/( vec2 a, vec2 b ) { vec2 result = { ( b.x != 0.0f ) ? a.x / b.x : 0.0f, ( b.y != 0.0f ) ? a.y / b.y : 0.0f }; return result; }
inline vec2 & operator+=( vec2 & a, vec2 b ) { a = a + b; return a; }
inline vec2 & operator-=( vec2 & a, vec2 b ) { a = a - b; return a; }
inline vec2 & operator*=( vec2 & a, flo32 f ) { a = a * f; return a; }
inline vec2 & operator*=( vec2 & a, vec2 b ) { a.x *= b.x; a.y *= b.y; return a; }
inline vec2 & operator/=( vec2 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec2( 0.0f, 0.0f );	return a; }
inline boo32 operator==( vec2 a, vec2 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ); return result; }
inline boo32 operator!=( vec2 a, vec2 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ); return result; }
inline flo32 getLengthSq( vec2 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ); return result; }
inline flo32 getLength( vec2 a ) { flo32 result = sqrtf( getLengthSq( a ) ); return result; }
inline vec2 getNormal( vec2 a ) { flo32 denom = getLength( a ); vec2 result = a / denom; return result; }
inline vec2 getPerp( vec2 a ) { vec2 result = { -a.y, a.x }; return result; }
inline vec2 getVector( flo32 degrees ) { vec2 result = { cosDegrees( degrees ), sinDegrees( degrees ) }; return result; }
inline flo32 dot( vec2 a, vec2 b ) { flo32 result = a.x * b.x + a.y * b.y; return result; }
inline vec2 lerp( vec2 a, flo32 t, vec2 b ) { vec2 result = Vec2( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ) ); return result; }

inline vec2 reflect( vec2 V, vec2 N ) {
    // NOTE: Assumes that N is normalized.
    vec2 result = V - ( 2.0f * dot( V, N ) ) * N;
    return result;
}

inline vec2
toVec2( flo32 length, flo32 radians ) {
    vec2 result = Vec2( cosf( radians ), sinf( radians ) ) * length;
    return result;
}

internal vec2
clamp01( vec2 value ) {
	vec2 result = {};
    result.x = clamp( value.x, 0.0f, 1.0f );
    result.y = clamp( value.y, 0.0f, 1.0f );
	return result;
}

//----------
// vec3 functions
//----------

inline vec3 Vec3( flo32 a ) { vec3 result = { a, a, a }; return result; }
inline vec3 Vec3( flo32 a, flo32 b, flo32 c ) { vec3 result = { a, b, c }; return result; }
inline vec3 Vec3( vec2 v, flo32 z ) { vec3 result = { v.x, v.y, z }; return result; }
inline vec3 operator+( vec3 a, vec3 b ) { vec3 result = { a.x + b.x, a.y + b.y, a.z + b.z }; return result; }
inline vec3 operator-( vec3 a, vec3 b ) { vec3 result = { a.x - b.x, a.y - b.y, a.z - b.z }; return result; }
inline vec3 operator-( vec3 a ) { vec3 result = { -a.x, -a.y, -a.z }; return result; }
inline vec3 operator*( vec3 a, flo32 f ) { vec3 result = { a.x * f, a.y * f, a.z * f }; return result; }
inline vec3 operator*( flo32 f, vec3 a ) { vec3 result = a * f; return result; }
inline vec3 operator/( vec3 a, flo32 f ) { vec3 result = {}; if( f != 0.0f ) { result = Vec3( a.x / f, a.y / f, a.z / f ); } return result; }
inline vec3 & operator+=( vec3 & a, vec3 b ) { a = a + b; return a; }
inline vec3 & operator-=( vec3 & a, vec3 b ) { a = a - b; return a; }
inline vec3 & operator*=( vec3 & a, flo32 f ) { a = a * f; return a; }
inline vec3 & operator/=( vec3 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec3( 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec3 a, vec3 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ); return result; }
inline boo32 operator!=( vec3 a, vec3 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ); return result; }
inline flo32 getLengthSq( vec3 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ); return result; }
inline flo32 getLength( vec3 a ) { flo32 result = sqrtf( getLengthSq( a ) ); return result; }
inline vec3  getNormal( vec3 a ) { flo32 denom = getLength( a ); vec3 result = a / denom; return result; }
inline vec3 cross( vec3 a, vec3 b ) {
	vec3 result = {};
	result.x = ( a.y * b.z ) - ( a.z * b.y );
	result.y = ( a.z * b.x ) - ( a.x * b.z );
	result.z = ( a.x * b.y ) - ( a.y * b.x );
	return result;
}
inline vec3 cross( vec3 a, vec3 b, vec3 c ) { vec3 result = cross( b - a, c - a ); return result; }
inline flo32 dot( vec3 a, vec3 b ) { flo32 result = a.x * b.x + a.y * b.y + a.z * b.z; return result; }
inline vec3 lerp( vec3 a, flo32 t, vec3 b ) { vec3 result = Vec3( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ) ); return result; }

//----------
// vec4 functions
//----------

inline vec4 Vec4() { vec4 result = {}; return result; }
inline vec4 Vec4( flo32 a, flo32 b, flo32 c, flo32 d ) { vec4 result = { a, b, c, d }; return result; }
inline vec4 Vec4( vec2 v, flo32 z, flo32 w ) { vec4 result = { v.x, v.y, z, w }; return result; }
inline vec4 Vec4( vec3 vector, flo32 w ) { vec4 result = { vector.x, vector.y, vector.z, w }; return result; }
inline vec4 Vec4( flo32 a ) { vec4 result = { a, a, a, a }; return result; }
inline vec4 operator+( vec4 a, vec4 b ) { vec4 result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; return result; }
inline vec4 operator-( vec4 a, vec4 b ) { vec4 result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; return result; }
inline vec4 operator*( vec4 a, vec4 b ) { vec4 result = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; return result; }
inline vec4 operator*( vec4 a, flo32 f ) { vec4 result = { a.x * f, a.y * f, a.z * f, a.w * f }; return result; }
inline vec4 operator*( flo32 f, vec4 a ) { vec4 result = a * f; return result; }
inline vec4 operator/( vec4 a, flo32 f ) { vec4 result = {}; if( f != 0.0f ) { result = Vec4( a.x / f, a.y / f, a.z / f, a.w / f ); } return result; }
inline vec4 operator-( vec4 a ) { vec4 result = { -a.x, -a.y, -a.z, -a.w }; return result; }
inline vec4 & operator+=( vec4 & a, vec4 b ) { a = a + b; return a; }
inline vec4 & operator-=( vec4 & a, vec4 b ) { a = a - b; return a; }
inline vec4 & operator*=( vec4 & a, flo32 f ) { a = a * f; return a; }
inline vec4 & operator*=( vec4 & a, vec4  b ) { a = a * b; return a; }
inline vec4 & operator/=( vec4 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec4 a, vec4 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w ); return result; }
inline boo32 operator!=( vec4 a, vec4 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ) || ( a.w != b.w ); return result; }
inline flo32 getLengthSq( vec4 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) + ( a.w * a.w ); return result; }
inline flo32 getLength( vec4 a ) { flo32 result = sqrtf( getLengthSq( a ) ); return result; }
inline vec4 getNormal( vec4 a ) { flo32 denom = getLength( a ); vec4 result = a / denom; return result; }
inline flo32 dot( vec4 a, vec4 b ) { flo32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; return result; }
inline vec4 lerp( vec4 a, flo32 t, vec4 b ) { vec4 result = Vec4( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ), lerp( a.w, t, b.w ) ); return result; }

internal vec4
toV4( uint8 r, uint8 g, uint8 b, uint8 a ) {
	vec4 result = { ( flo32 )r / 255.0f, ( flo32 )g / 255.0f, ( flo32 )b / 255.0f, ( flo32 )a / 255.0f };
	return result;
}

internal vec4
modAlpha( vec4 color, flo32 alpha ) {
    vec4 result = color;
    result.rgb *= alpha;
    result.a    = alpha;
    return result;
}

//----------
// rect functions
//----------

inline rect RectMin() {
    rect result = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };
    return result;
}

inline rect Rect( flo32 left, flo32 bottom, flo32 right, flo32 top ) { rect result = { left, bottom, right, top }; return result; }
inline vec2 getP( rect r, vec2 t ) {
    vec2 result = {};
    result.x = r.left   + ( r.right - r.left   ) * t.x;
    result.y = r.bottom + ( r.top   - r.bottom ) * t.y;
    return result;
}
inline flo32 getWidth(  rect r ) { flo32 result = ( r.right - r.left ); return result; }
inline flo32 getHeight( rect r ) { flo32 result = ( r.top - r.bottom ); return result; }
inline vec2 getDim( rect r ) { vec2 result = { getWidth( r ), getHeight( r ) }; return result; }
inline vec2 getHalfDim( rect r ) { vec2 result = getDim( r ) * 0.5f; return result; }
inline rect operator*( rect r, flo32 t ) { rect result = r;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        result.elem[ iter ] *= t;
    }
    return result;
}
inline vec2 getTL( rect r ) { vec2 result = Vec2( r.left,  r.top    ); return result; }
inline vec2 getTR( rect r ) { vec2 result = Vec2( r.right, r.top    ); return result; }
inline vec2 getBL( rect r ) { vec2 result = Vec2( r.left,  r.bottom ); return result; }
inline vec2 getBR( rect r ) { vec2 result = Vec2( r.right, r.bottom ); return result; }
inline vec2 getTC( rect r ) { vec2 result = Vec2( ( r.left + r.right ) * 0.5f, r.top    ); return result; }
inline vec2 getBC( rect r ) { vec2 result = Vec2( ( r.left + r.right ) * 0.5f, r.bottom ); return result; }
inline vec2 getLC( rect r ) { vec2 result = Vec2( r.left,  ( r.bottom + r.top ) * 0.5f );  return result; }
inline vec2 getRC( rect r ) { vec2 result = Vec2( r.right, ( r.bottom + r.top ) * 0.5f );  return result; }
inline rect rectTLD( vec2 topLeft,     vec2 dim ) { rect result = Rect( topLeft.x,             topLeft.y - dim.y,  topLeft.x + dim.x,    topLeft.y             ); return result; }
inline rect rectTRD( vec2 topRight,    vec2 dim ) { rect result = Rect( topRight.x - dim.x,    topRight.y - dim.y, topRight.x,           topRight.y            ); return result; }
inline rect rectBLD( vec2 bottomLeft,  vec2 dim ) { rect result = Rect( bottomLeft.x,          bottomLeft.y,       bottomLeft.x + dim.x, bottomLeft.y  + dim.y ); return result; }
inline rect rectBRD( vec2 bottomRight, vec2 dim ) { rect result = Rect( bottomRight.x - dim.x, bottomRight.y,      bottomRight.x,        bottomRight.y + dim.y ); return result; }
inline rect rectLCD( vec2   leftCenter, vec2 dim ) { flo32 halfDimY = dim.y * 0.5f; rect result = Rect(  leftCenter.x,  leftCenter.y - halfDimY,  leftCenter.x + dim.x,           leftCenter.y + halfDimY ); return result; }
inline rect rectRCD( vec2  rightCenter, vec2 dim ) { flo32 halfDimY = dim.y * 0.5f; rect result = Rect( rightCenter.x - dim.x,         rightCenter.y - halfDimY, rightCenter.x, rightCenter.y + halfDimY ); return result; }
inline rect rectBCD( vec2 bottomCenter, vec2 dim ) { flo32 halfDimX = dim.x * 0.5f; rect result = Rect( bottomCenter.x - halfDimX, bottomCenter.y,      bottomCenter.x + halfDimX, bottomCenter.y + dim.y ); return result; }
inline rect rectTCD( vec2    topCenter, vec2 dim ) { flo32 halfDimX = dim.x * 0.5f; rect result = Rect(    topCenter.x - halfDimX, topCenter.y - dim.y,    topCenter.x + halfDimX, topCenter.y            ); return result; }
inline rect rectCD(  vec2 center, vec2 dim ) { vec2 halfDim = dim * 0.5f; rect result = Rect( center.x - halfDim.x, center.y - halfDim.y, center.x + halfDim.x, center.y + halfDim.y ); return result; }
inline rect rectCHD( vec2 center, vec2 halfDim ) { rect result = Rect( center.x - halfDim.x, center.y - halfDim.y, center.x + halfDim.x, center.y + halfDim.y ); return result; }
inline rect rectCR(  vec2 center, flo32 radius ) { rect result = Rect( center.x - radius, center.y - radius, center.x + radius, center.y + radius ); return result; }
inline rect rectMM(  vec2 min, vec2 max ) { rect result = Rect( min.x, min.y, max.x, max.y ); return result; }
inline rect addDim( rect r, flo32 x0, flo32 y0, flo32 x1, flo32 y1 ) { rect result = Rect( r.left - x0, r.bottom - y0, r.right + x1, r.top + y1 ); return result; }
inline rect addDim( rect r, vec2 dim ) { rect result = Rect( r.left - dim.x, r.bottom - dim.y, r.right + dim.x, r.top + dim.y ); return result; }
inline rect addRadius( rect r, flo32 radius ) { rect result = Rect( r.left - radius, r.bottom - radius, r.right + radius, r.top + radius ); return result; }
inline vec2 getCenter( rect r ) { vec2 result = Vec2( ( r.left + r.right ) * 0.5f, ( r.bottom + r.top ) * 0.5f ); return result; }
inline rect rectCompare( vec2 point0, vec2 point1 ) {
    boo32 xMin_usePoint0 = ( point0.x < point1.x );
    boo32 yMin_usePoint0 = ( point0.y < point1.y );
    rect result = Rect(
                       ( xMin_usePoint0 ? point0.x : point1.x ), ( yMin_usePoint0 ? point0.y : point1.y ),
                       ( xMin_usePoint0 ? point1.x : point0.x ), ( yMin_usePoint0 ? point1.y : point0.y )
                       );
    return result;
}
inline rect addOffset( rect r, vec2 offset ) { rect result = Rect( r.left + offset.x, r.bottom + offset.y, r.right + offset.x, r.top + offset.y ); return result; }

inline boo32 isInBound( vec2 point, rect bound ) { boo32 result = ( point.x >= bound.left ) && ( point.y >= bound.bottom ) && ( point.x <= bound.right ) && ( point.y <= bound.top ); return result; }

internal rect
scaleRect( rect R, vec2 scale ) {
    vec2 center = getCenter( R );
    vec2 dim    = getDim( R ) * scale;
    rect result = rectCD( center, dim );
    return result;
}

internal vec2
toDirection2D( flo32 radians ) {
    vec2 result = Vec2( cosf( radians ), sinf( radians ) );
    return result;
}

internal flo32
toRadians( vec2 v ) {
    flo32 result = atan2f( v.y, v.x );
    if( result < 0.0f ) {
        result += ( PI * 2.0f );
    }
    return result;
}

internal vec2
RandomV2() {
    vec2 result = { RandomF32(), RandomF32() };
    return result;
}

internal vec2
RandomDirection2D() {
    flo32 radians = RandomF32() * 2.0f * PI;
    vec2 result = toDirection2D( radians );
    return result;
}

internal vec2
getT( rect a, vec2 t ) {
    vec2 result = {};
    result.x = lerp( a.left,   t.x, a.right );
    result.y = lerp( a.bottom, t.y, a.top   );
    return result;
}

internal vec2
getRandomPoint( rect a ) {
    vec2 t = RandomV2();
    vec2 result = {};
    result.x = lerp( a.left,   t.x, a.right );
    result.y = lerp( a.bottom, t.y, a.top   );
    return result;
}

internal vec2
clampToBound( vec2 P, rect A ) {
    vec2 result = P;
    result.x = maxValue( result.x, A.left   );
    result.x = minValue( result.x, A.right  );
    result.y = maxValue( result.y, A.bottom );
    result.y = minValue( result.y, A.top    );
    return result;
}

internal RECT_PERIMETER
getClosestPerimeterPoint( rect A, vec2 P ) {
    RECT_PERIMETER result = {};
    result.position = P;
    
    vec2 dist  = {};
    vec2 value = {};
    vec2 norm  = {};
    
    flo32 dist_left  = fabsf( P.x - A.left  );
    flo32 dist_right = fabsf( P.x - A.right );
    if( dist_left <= dist_right ) {
        dist.x  = dist_left;
        value.x = A.left;
        norm.x  = -1.0f;
    } else {
        dist.x  = dist_right;
        value.x = A.right;
        norm.x  = 1.0f;
    }
    
    flo32 dist_bottom = fabsf( P.y - A.bottom );
    flo32 dist_top    = fabsf( P.y - A.top    );
    if( dist_bottom <= dist_top ) {
        dist.y  = dist_bottom;
        value.y = A.bottom;
        norm.y  = -1.0f;
    } else {
        dist.y  = dist_top;
        value.y = A.top;
        norm.y  = 1.0f;
    }
    
    if( dist.x <= dist.y ) {
        result.position.x = value.x;
        result.normal     = Vec2( norm.x, 0.0f );
    } else {
        result.position.y = value.y;
        result.normal     = Vec2( 0.0f, norm.y );
    }
    
    return result;
}

internal rect
getIntersect( rect A, rect B ) {
    Assert( !( ( A.right < B.left ) || ( A.left > B.right ) || ( A.top < B.bottom ) || ( A.bottom > B.top ) ) );
    
    rect result = {};
    result.left   = maxValue( A.left,   B.left   );
    result.right  = minValue( A.right,  B.right  );
    result.bottom = maxValue( A.bottom, B.bottom );
    result.top    = minValue( A.top,    B.top    );
    return result;
}

//----------
// oriented rect functions
//----------

internal vec2 getBL( orect R ) { vec2 result = R.center - R.xAxis * R.halfDim.x - R.yAxis * R.halfDim.y;  return result; }
internal vec2 getBR( orect R ) { vec2 result = R.center + R.xAxis * R.halfDim.x - R.yAxis * R.halfDim.y;  return result; }
internal vec2 getTL( orect R ) { vec2 result = R.center - R.xAxis * R.halfDim.x + R.yAxis * R.halfDim.y;  return result; }
internal vec2 getTR( orect R ) { vec2 result = R.center + R.xAxis * R.halfDim.x + R.yAxis * R.halfDim.y;  return result; }
internal vec2 getLC( orect R ) { vec2 result = R.center - R.xAxis * R.halfDim.x; return result; }
internal vec2 getRC( orect R ) { vec2 result = R.center + R.xAxis * R.halfDim.x; return result; }
internal vec2 getBC( orect R ) { vec2 result = R.center - R.yAxis * R.halfDim.y; return result; }
internal vec2 getTC( orect R ) { vec2 result = R.center + R.yAxis * R.halfDim.y; return result; }

internal orect
expand( orect R, flo32 left, flo32 bottom, flo32 right, flo32 top ) {
    vec2 offset = Vec2( -left * 0.5f + right * 0.5f, -bottom * 0.5f + top * 0.5f );
    
    orect result = R;
    result.center  += ( offset.x * R.xAxis + offset.y * R.yAxis );
    result.halfDim += Vec2( ( left + right ) * 0.5f, ( bottom + top ) * 0.5f );
    return result;
}

internal orect expandl( orect R, flo32 t ) { orect result = expand( R, t,    0.0f, 0.0f, 0.0f ); return result; }
internal orect expandb( orect R, flo32 t ) { orect result = expand( R, 0.0f, t,    0.0f, 0.0f ); return result; }
internal orect expandr( orect R, flo32 t ) { orect result = expand( R, 0.0f, 0.0f, t,    0.0f ); return result; }
internal orect expandt( orect R, flo32 t ) { orect result = expand( R, 0.0f, 0.0f, 0.0f, t    ); return result; }
internal orect expandh( orect R, flo32 t ) { orect result = R; result.halfDim.x += t; return result; }

internal orect
orectCD( vec2 center, vec2 dim, flo32 radians = 0.0f ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    result.center  = center;
    result.halfDim = dim * 0.5f;
    return result;
}

internal orect
orectCHD( vec2 center, vec2 halfDim, flo32 radians = 0.0f ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    result.center  = center;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectTLD( vec2 topLeft, vec2 dim, flo32 radians = 0.0f ) {
    vec2 halfDim = dim * 0.5f;
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.center = topLeft + xAxis * halfDim.x - yAxis * halfDim.y;
    result.xAxis  = xAxis;
    result.yAxis  = yAxis;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectTRD( vec2 topRight, vec2 dim, flo32 radians = 0.0f ) {
    vec2 halfDim = dim * 0.5f;
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.center = topRight - xAxis * halfDim.x - yAxis * halfDim.y;
    result.xAxis  = xAxis;
    result.yAxis  = yAxis;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectBLD( vec2 bottomLeft, vec2 dim, flo32 radians = 0.0f ) {
    vec2 halfDim = dim * 0.5f;
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.center = bottomLeft + xAxis * halfDim.x + yAxis * halfDim.y;
    result.xAxis  = xAxis;
    result.yAxis  = yAxis;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectBRD( vec2 topLeft, vec2 dim, flo32 radians = 0.0f ) {
    vec2 halfDim = dim * 0.5f;
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.center = topLeft - xAxis * halfDim.x + yAxis * halfDim.y;
    result.xAxis  = xAxis;
    result.yAxis  = yAxis;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectRHD( vec2 bottomRight, vec2 topRight, flo32 halfDimX ) {
    vec2 V = topRight - bottomRight;
    
    vec2 halfDim = Vec2( halfDimX, getLength( V ) * 0.5f );
    vec2 yAxis = getNormal( V );
    vec2 xAxis = -getPerp( yAxis );
    
    orect result = {};
    result.center = topRight - xAxis * halfDim.x - yAxis * halfDim.y;
    result.xAxis  = xAxis;
    result.yAxis  = yAxis;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectTHD( vec2 topRight, vec2 topLeft, flo32 halfDimY ) {
    vec2 V = topLeft - topRight;
    
    vec2 halfDim = Vec2( getLength( V ) * 0.5f, halfDimY );
    vec2 xAxis = -getNormal( V );
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.center = topLeft + xAxis * halfDim.x - yAxis * halfDim.y;
    result.xAxis  = xAxis;
    result.yAxis  = yAxis;
    result.halfDim = halfDim;
    return result;
}

internal boo32
isInBound( vec2 P, orect bound ) {
    vec2 V = P - bound.center;
    boo32 result = ( fabsf( dot( V, bound.xAxis ) ) <= bound.halfDim.x ) && ( fabsf( dot( V, bound.yAxis ) ) <= bound.halfDim.y );
    return result;
}

//----------
// line segment functions
//----------

internal LINE_SEG
LineSeg( vec2 origin, flo32 length, flo32 radians ) {
    vec2 V = Vec2( cosf( radians ), sinf( radians ) ) * length;
    
    LINE_SEG result = {};
    result.P = origin;
    result.Q = origin + V;
    return result;
}

internal LINE_SEG
LineSeg( vec2 P, vec2 Q ) {
    LINE_SEG result = {};
    result.P = P;
    result.Q = Q;
    return result;
}

internal LINE_SEG
LineSegQP( vec2 origin, flo32 length, flo32 radians ) {
    LINE_SEG result = LineSeg( origin, length, radians );
    _swap( vec2, result.P, result.Q );
    return result;
}

internal LINE_SEG
expand( LINE_SEG A, flo32 t ) {
    vec2 N = getNormal( A.Q - A.P ) * t;
    LINE_SEG result = {};
    result.P = A.P - N;
    result.Q = A.Q + N;
    return result;
}

//----------
// circle functions
//----------

internal circ
Circ( vec2 center, flo32 radius ) {
    circ result = {};
    result.center = center;
    result.radius = radius;
    return result;
}

internal circ
Circle( vec2 center, flo32 radius ) {
    circ result = {};
    result.center = center;
    result.radius = radius;
    return result;
}

internal boo32
isInBound( vec2 p, circ c ) {
    boo32 result = false;
    
    flo32 radiusSq = c.radius * c.radius;
    flo32 distSq   = getLengthSq( p - c.center );
    if( distSq <= radiusSq ) {
        result = true;
    }
    return result;
}

//----------
// capsule2 functions
//----------

internal CAPSULE2
Capsule2( vec2 P, vec2 Q, flo32 radius ) {
    CAPSULE2 result = {};
    result.P = P;
    result.Q = Q;
    result.radius = radius;
    return result;
}

//----------
// 3x3 matrix functions
//----------

inline mat3 Mat3( vec3 a, vec3 b, vec3 c ) {
	mat3 result = { a, b, c };
	return result;
}

inline vec3 row( mat3 matrix, int32 rowIndex ) {
	vec3 result = {};
	for( int32 counter = 0; counter < 3; counter++ ) {
		result.elem[ counter ] = matrix.elem[ counter * 3 + rowIndex ];
	}
	return result;
}

internal mat3
mat3_transpose( mat3 a ) {
	mat3 result = {};
	for( int32 index = 0; index < 3; index++ ) {
		result.column[ index ] = row( a, index );
	}
	return result;
}

internal mat3
mat3_inverse( mat3 a ) {
    vec3 U = a.column[ 0 ];
    vec3 V = a.column[ 1 ];
    vec3 W = a.column[ 2 ];
    
    flo32 denom = dot( U, cross( V, W ) );
    Assert( denom != 0.0f );
    vec3 A = cross( V, W ) / denom;
    vec3 B = cross( W, U ) / denom;
    vec3 C = cross( U, V ) / denom;
    
    mat3 result = mat3_transpose( Mat3( A, B, C ) );
    return result;
}

internal mat3
mat3_byRow( vec3 rowA, vec3 rowB, vec3 rowC ) {
	mat3 result = { rowA.x, rowB.x, rowC.x, rowA.y, rowB.y, rowC.y, rowA.z, rowB.z, rowC.z };
	return result;
}

// internal vec3
// operator*( mat3 a, vec3 b ) {
// vec3 result = {};
// for( int32 index = 0; index < 3; index++ ) {
// result.elem[ index ] = dot( row( a, index ), b );
// }
// return result;
// }

internal vec3
operator*( mat3 a, vec3 b ) {
	vec3 result = {};
    for( int32 index = 0; index < 3; index++ ) {
        result.elem[ index ] = dot( row( a, index ), b );
    }
	return result;
}

internal mat3
operator*( mat3 a, mat3 b ) {
    mat3 result = {};
    for( int32 columnIndex = 0; columnIndex < 3; columnIndex++ ) {
        for( int32 rowIndex = 0; rowIndex < 3; rowIndex++ ) {
            int32 elemIndex = columnIndex * 3 + rowIndex;
            result.elem[ elemIndex ] = dot( row( a, rowIndex ), b.column[ columnIndex ] );
        }
    }
    return result;
}

internal mat3
mat3_toRotation( flo32 x, flo32 y, flo32 z, flo32 radians ) {
	flo32 cos0 = cosf( radians );
	flo32 sin0 = sinf( radians );
	flo32 invCos = 1.0f - cos0;
	
	flo32 xyInv = x * y * invCos;
	flo32 xzInv = x * z * invCos;
	flo32 yzInv = y * z * invCos;
	flo32 x2Inv = x * x * invCos;
	flo32 y2Inv = y * y * invCos;
	flo32 z2Inv = z * z * invCos;
	flo32 xSin  = x * sin0;
	flo32 ySin  = y * sin0;
	flo32 zSin  = z * sin0;
	
	mat3 result = {
		x2Inv + cos0, xyInv + zSin, xzInv - ySin,
		xyInv - zSin, y2Inv + cos0, yzInv + xSin,
		xzInv + ySin, yzInv - xSin, z2Inv + cos0,
	};
	return result;
}

internal flo32
mat3_cofactor( mat3 A, uint32 row_index, uint32 column_index ) {
    // cofactor = -1^i+j * det( minor )
    flo32  mat2_elem[ 4 ] = {};
    uint32 dest_index = 0;
    for( uint32 j = 0; j < 3; j++ ) {
        if( j != column_index ) {
            for( uint32 i = 0; i < 3; i++ ) {
                if( i != row_index ) {
                    mat2_elem[ dest_index++ ] = A.elem[ j * 3 + i ];
                }
            }
        }
    }
    Assert( dest_index == 4 );
    
    flo32 result = mat2_elem[ 0 ] * mat2_elem[ 3 ] - mat2_elem[ 1 ] * mat2_elem[ 2 ];
    if( ( ( row_index + column_index ) % 2 ) == 1 ) {
        result = -result;
    }
    return result;
}

internal flo32
mat3_determinant( mat3 A ) {
    // det = sum of element * cofactor
    flo32 result = 0.0f;
    for( uint32 iter = 0; iter < 3; iter++ ) {
        vec3 column = A.column[ iter ];
        
        flo32 element  = column.z;
        flo32 cofactor = mat3_cofactor( A, 2, iter );
        
        result += ( element * cofactor );
    }
    return result;
}

//----------
// 4x4 matrix functions
//----------

inline mat4
operator-( mat4 a ) {
    mat4 result = {};
    for( uint32 iElem = 0; iElem < 16; iElem++ ) {
        result.elem[ iElem ] = -a.elem[ iElem ];
    }
    return result;
}

inline vec4 getRow( mat4 matrix, int32 rowIndex ) {
	vec4 result = {};
	for( int32 counter = 0; counter < 4; counter++ ) {
		result.elem[ counter ] = matrix.elem[ counter * 4 + rowIndex ];
	}
	return result;
}

inline mat4 mat4_identity() {
    mat4 result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return result;
}

internal mat4
mat4_frustum( flo32 left, flo32 right, flo32 bottom, flo32 top, flo32 nearPlane, flo32 farPlane ) {
	flo32 width = right - left;
	flo32 height = top - bottom;
	flo32 depth = farPlane - nearPlane;
	flo32 nearPlane2x = nearPlane * 2.0f;
	
	mat4 result = {
        nearPlane2x / width, 							  0.0f, 										 0.0f,  0.0f,	// column 1
        0.0f, 		 nearPlane2x / height, 										 0.0f,  0.0f,	// column 2
		( right + left ) / width, ( top + bottom ) / height, -( farPlane + nearPlane ) / depth, -1.0f,	// column 3
        0.0f, 							  0.0f, 	 -nearPlane2x * farPlane / depth,  0.0f,	// column 4
	};
	
	return result;
}

// NOTE: opengl
// internal mat4
// mat4_perspective( vec2 window_dim, flo32 fovAngle, flo32 nearPlane, flo32 farPlane ) {
// flo32 aspectRatio = window_dim.x / window_dim.y;
// flo32 fovHalfAngle = fovAngle * 0.5f;
// flo32 cot = cosDegrees( fovHalfAngle ) / sinDegrees( fovHalfAngle );
// flo32 depth = nearPlane - farPlane;

// mat4 result = {
// cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
// 0.0f, cot, 0.0f, 0.0f,	// column 2
// 0.0f, 0.0f, ( farPlane + nearPlane ) / depth, -1.0f,	// column 3
// 0.0f, 0.0f, ( 2.0f * nearPlane * farPlane ) / depth,  0.0f,	// column 4
// };
// return result;
// }

// NOTE: direct3d
internal mat4
mat4_perspective( vec2 window_dim, flo32 fovRadians, flo32 nearPlane, flo32 farPlane ) {
	flo32 aspectRatio = window_dim.x / window_dim.y;
	flo32 fovHalfRadians = fovRadians * 0.5f;
	flo32 cot = cosf( fovHalfRadians ) / sinf( fovHalfRadians );
	flo32 depth = farPlane - nearPlane;
	
	mat4 result = {
		cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, cot, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, farPlane / depth, 1.0f,	// column 3
		0.0f, 0.0f, -( farPlane * nearPlane ) / depth, 0.0f,	// column 4
	};
	return result;
}

// NOTE: opengl z-coord [-1..1]
// internal mat4
// mat4_orthographic( flo32 left, flo32 bottom, flo32 right, flo32 top, flo32 nearPlane, flo32 farPlane ) {
// flo32 width = right - left;
// flo32 height = top - bottom;
// flo32 depth = farPlane - nearPlane;

// mat4 result = {
// 2.0f / width, 0.0f, 0.0f, 0.0f,	// column 1
// 0.0f, 2.0f / height, 0.0f, 0.0f,	// column 2
// 0.0f, 0.0f, -2.0f / depth, 0.0f,	// column 3
// -( right + left ) / width, -( top + bottom ) / height, -( farPlane + nearPlane ) / depth, 1.0f,	// column 4
// };
// return result;
// }

// NOTE: direct3d z-coord [0..1]
internal mat4
mat4_orthographic( flo32 left, flo32 bottom, flo32 right, flo32 top ) {
	flo32 width = right - left;
	flo32 height = top - bottom;
	
	mat4 result = {
		2.0f / width, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, 2.0f / height, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, 1.0f, 0.0f,	// column 3
		-( right + left ) / width, -( top + bottom ) / height, 0.0f, 1.0f,	// column 4
	};
	return result;
}

internal mat4
mat4_orthographic( rect bound ) {
	mat4 result = mat4_orthographic( bound.left, bound.bottom, bound.right, bound.top );
	return result;
}

internal mat4
mat4_transpose( mat4 a ) {
	mat4 result = {};
	for( int32 index = 0; index < 4; index++ ) {
		result.column[ index ] = getRow( a, index );
	}
	return result;
}

internal mat4
mat4_translate( flo32 x, flo32 y, flo32 z ) {
	mat4 result = mat4_identity();
	result.column[3] = Vec4( x, y, z, 1.0f );
	return result;
}

inline mat4
mat4_translate( vec2 xy, flo32 z = 0.0f ) {
	mat4 result = mat4_translate( xy.x, xy.y, z );
	return result;
}

inline mat4
mat4_translate( vec3 a ) {
	mat4 result = mat4_translate( a.x, a.y, a.z );
	return result;
}

inline mat4
mat4_rotate( vec3 axis, flo32 radians ) {
	axis = getNormal( axis );
	
	flo32 cosValue = cosf( radians );
	flo32 sinValue = sinf( radians );
	flo32 invCos = 1.0f - cosValue;
	
	flo32 xyInv = axis.x * axis.y * invCos;
	flo32 xzInv = axis.x * axis.z * invCos;
	flo32 yzInv = axis.y * axis.z * invCos;
	flo32 x2Inv = squareValue( axis.x ) * invCos;
	flo32 y2Inv = squareValue( axis.y ) * invCos;
	flo32 z2Inv = squareValue( axis.z ) * invCos;
	flo32 xSin = axis.x * sinValue;
	flo32 ySin = axis.y * sinValue;
	flo32 zSin = axis.z * sinValue;
	
	mat4 result = {
		x2Inv + cosValue, xyInv + zSin, xzInv - ySin, 0.0f,
		xyInv - zSin, y2Inv + cosValue, yzInv + xSin, 0.0f,
		xzInv + ySin, yzInv - xSin, z2Inv + cosValue, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return result;
}

internal mat4
operator*( mat4 a, mat4 b ) {
	mat4 result = {};
	for( int32 columnIndex = 0; columnIndex < 4; columnIndex++ ) {
		for( int32 rowIndex = 0; rowIndex < 4; rowIndex++ ) {
			int32 elemIndex = columnIndex * 4 + rowIndex;
			result.elem[ elemIndex ] = dot( getRow( a, rowIndex ), b.column[ columnIndex ] );
		}
	}
	return result;
}

internal vec4
operator*( mat4 a, vec4 b ) {
	vec4 result = {};
	for( int32 elemIndex = 0; elemIndex < 4; elemIndex++ ) {
		result.elem[ elemIndex ] = dot( getRow( a, elemIndex ), b );
	}
	return result;
}

inline mat4 & operator*=( mat4 & a, mat4 b ) { a = a * b; return a; }

internal mat4
mat4_scale( flo32 xScale, flo32 yScale, flo32 zScale ) {
	mat4 result = { xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, zScale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
	};
	return result;
}

internal mat4
mat4_scale( vec2 xyScale, flo32 zScale ) {
    mat4 result = mat4_scale( xyScale.x, xyScale.y, zScale );
    return result;
}

inline mat4
mat4_scale( vec3 scale ) {
	mat4 result = mat4_scale( scale.x, scale.y, scale.z );
	return result;
}

inline mat4
mat4_reflect( vec3 normal ) {
    vec3 X = Vec3( 1.0f, 0.0f, 0.0f );
    vec3 Y = Vec3( 0.0f, 1.0f, 0.0f );
    vec3 Z = Vec3( 0.0f, 0.0f, 1.0f );
    
    vec3 X0 = X - normal * ( 2.0f * dot( X, normal ) );
    vec3 Y0 = Y - normal * ( 2.0f * dot( Y, normal ) );
    vec3 Z0 = Z - normal * ( 2.0f * dot( Z, normal ) );
    
    mat4 result = {};
    result.column[ 0 ] = Vec4( X0, 0.0f );
    result.column[ 1 ] = Vec4( Y0, 0.0f );
    result.column[ 2 ] = Vec4( Z0, 0.0f );
    result.column[ 3 ] = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    return result;
}

internal mat4
operator*( mat4 a, flo32 f ) {
    mat4 result = a;
    for( uint32 elem_index = 0; elem_index < 16; elem_index++ ) {
        result.elem[ elem_index ] *= f;
    }
    return result;
}

internal mat4
mat4_column( vec3 X, vec3 Y, vec3 Z, vec3 W = Vec3( 0.0f, 0.0f, 0.0f ) ) {
    mat4 result = {};
    result.column[ 0 ] = Vec4( X, 0.0f );
    result.column[ 1 ] = Vec4( Y, 0.0f );
    result.column[ 2 ] = Vec4( Z, 0.0f );
    result.column[ 3 ] = Vec4( W, 1.0f );
    return result;
}

internal flo32
mat4_cofactor( mat4 A, uint32 row_index, uint32 column_index ) {
    // cofactor = -1^i+j * det( minor )
    mat3   minor = {};
    uint32 dest_index = 0;
    for( uint32 j = 0; j < 4; j++ ) {
        if( j != column_index ) {
            for( uint32 i = 0; i < 4; i++ ) {
                if( i != row_index ) {
                    minor.elem[ dest_index++ ] = A.elem[ j * 4 + i ];
                }
            }
        }
    }
    Assert( dest_index == 9 );
    
    flo32 result = mat3_determinant( minor );
    if( ( ( row_index + column_index ) % 2 ) == 1 ) {
        result = -result;
    }
    return result;
}

internal flo32
mat4_determinant( mat4 A ) {
    // sum of element * cofactor
    flo32 result = 0.0f;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec4 column = A.column[ iter ];
        
        flo32 element  = column.w;
        flo32 cofactor = mat4_cofactor( A, 3, iter );
        
        result += ( element * cofactor );
    }
    return result;
}

internal mat4
mat4_cofactor( mat4 A ) {
    mat4 result = {};
    for( uint32 j = 0; j < 4; j++ ) {
        for( uint32 i = 0; i < 4; i++ ) {
            result.elem[ j * 4 + i ] = mat4_cofactor( A, i, j );
        }
    }
    return result;
}

internal mat4
mat4_adjugate( mat4 A ) {
    mat4 cofactor = mat4_cofactor( A );
    mat4 result   = mat4_transpose( cofactor );
    return result;
}

internal mat4
mat4_inverse( mat4 A ) {
    // 1 / |A| * M
    flo32 det = mat4_determinant( A );
    mat4  M   = mat4_adjugate   ( A );
    
    Assert( det != 0.0f );
    mat4 result = M * ( 1.0f / det );
    return result;
}

#endif	// STD_INCLUDE_FUNC
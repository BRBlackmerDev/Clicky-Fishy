
//----------
// vec3 check functions
//----------

#define T_VALID_EPSILON ( -0.0001f )
#define RAY_INTERSECT__VALID_NEG_DIST_EPSILON ( 2.0f )
#define DOT__DEGENERATE_EPSILON  ( 0.000001f )
#define DOT__COLLINEAR_EPSILON   ( 0.000001f )
#define DOT__ORTHOGONAL_EPSILON  ( 0.000001f )

internal boo32
isDegenerate( vec3 V ) {
    flo32 dotVV  = dot( V, V );
    boo32 result = ( dotVV <= DOT__DEGENERATE_EPSILON );
    return result;
}

internal boo32
isOrthogonalTo( vec3 U, vec3 V ) {
    flo32 dotUV = dot( U, V );
    dotUV = fabsf( dotUV );
    boo32 result = ( dotUV <= DOT__ORTHOGONAL_EPSILON );
    return result;
}

internal boo32
isCollinearTo( vec3 U, vec3 V ) {
    flo32 dotUV = dot( U, V );
    boo32 result = ( fabsf( dotUV ) >= ( 1.0f - DOT__COLLINEAR_EPSILON ) );
    return result;
}

internal boo32
isOppositeTo( vec3 U, vec3 V ) {
    flo32 dotUV = dot( U, V );
    boo32 result = ( dotUV < -DOT__ORTHOGONAL_EPSILON );
    return result;
}

//----------
// ray2 functions
//----------

struct RAY2 {
    vec2 origin;
    vec2 vector;
};

struct RAY2_INTERSECT {
    boo32 isValid;
    flo32 t;
    vec2  P;
    vec2  N;
    
    flo32 denom;
    flo32 tMin;
    flo32 s;
};

internal RAY2
Ray2( vec2 origin, vec2 vector ) {
    RAY2 result = {};
    result.origin = origin;
    result.vector = vector;
    return result;
}

internal RAY2_INTERSECT
Ray2IntersectInit() {
    RAY2_INTERSECT result = {};
    result.t = FLT_MAX;
    return result;
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, vec2 P, vec2 Q ) {
    RAY2_INTERSECT result = {};
    
    vec2 V = Q - P;
    vec2 W = P - ray.origin;
    result.N = getNormal( getPerp( V ) );
    
    result.denom = dot( result.N, ray.vector );
    if( result.denom <= -DOT__COLLINEAR_EPSILON ) {
        result.tMin = ( -RAY_INTERSECT__VALID_NEG_DIST_EPSILON / getLength( ray.vector ) );
        result.t = dot( result.N, W ) / result.denom;
        if( result.t >= result.tMin ) {
            result.P = ray.origin + ray.vector * result.t;
            result.s = dot( result.P - P, V ) / dot( V, V );
            if( ( result.s >= -0.01f ) && ( result.s <= 1.01f ) ) {
                result.isValid = true;
            }
        }
    }
    return result;
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, LINE_SEG line ) {
    RAY2_INTERSECT result = doesIntersect( ray, line.P, line.Q );
    return result;
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, rect bound ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = Vec2( bound.left,  bound.bottom );
    point[ 1 ] = Vec2( bound.left,  bound.top    );
    point[ 2 ] = Vec2( bound.right, bound.top    );
    point[ 3 ] = Vec2( bound.right, bound.bottom );
    point[ 4 ] = Vec2( bound.left,  bound.bottom );
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        RAY2_INTERSECT intersect = doesIntersect( ray, P, Q );
        if( ( intersect.isValid ) && ( intersect.t < result.t ) ) {
            result = intersect;
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, orect bound ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 xAxis = bound.xAxis * bound.halfDim.x;
    vec2 yAxis = bound.yAxis * bound.halfDim.y;
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = bound.center - xAxis - yAxis;
    point[ 1 ] = bound.center - xAxis + yAxis;
    point[ 2 ] = bound.center + xAxis + yAxis;
    point[ 3 ] = bound.center + xAxis - yAxis;
    point[ 4 ] = point[ 0 ];
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        RAY2_INTERSECT intersect = doesIntersect( ray, P, Q );
        if( ( intersect.isValid ) && ( intersect.t < result.t ) ) {
            result = intersect;
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, circ C ) {
    RAY2_INTERSECT result = {};
    
    vec2 U = getNormal( ray.vector );
    vec2 V = ray.origin - C.center;
    
    flo32 dotUV = dot( U, V );
    
    flo32 discriminant = ( dotUV * dotUV ) + ( C.radius * C.radius ) - dot( V, V );
    result.denom = discriminant;
    if( discriminant >= 0.0f ) {
        flo32 tMin = -RAY_INTERSECT__VALID_NEG_DIST_EPSILON;
        result.tMin = tMin;
        flo32 t = -dotUV - sqrtf( discriminant );
        result.t = t / getLength( ray.vector );
        //NOTE: only checks if the near side of the sphere is valid
        //NOTE: t = -dotUV + sqrtf( disc ) would check the far side of the sphere
        if( t >= tMin ) {
            result.isValid = true;
            result.P       = ray.origin + ray.vector * result.t;
            result.N       = getNormal( result.P - C.center );
        }
    }
    
    return result;
}

internal void
logCollision( RAY2_INTERSECT intersect ) {
    if( globalVar_debugSystem.log_isEnabled ) {
        CONSOLE_VALUE( boo32,  intersect.isValid );
        CONSOLE_VALUE( vec2,   intersect.N );
        CONSOLE_VALUE( flo32,  intersect.denom );
        CONSOLE_VALUE( flo32,  intersect.tMin );
        CONSOLE_VALUE( flo32,  intersect.t );
        CONSOLE_VALUE( vec2,   intersect.P );
        CONSOLE_VALUE( flo32,  intersect.s );
    }
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, CAPSULE2 cap, boo32 doLog = false ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 xAxis = getNormal( cap.Q - cap.P );
    vec2 yAxis = getPerp( xAxis );
    yAxis *= cap.radius;
    
    RAY2_INTERSECT intersect[ 2 ] = {};
    intersect[ 0 ] = doesIntersect( ray, LineSeg( cap.P + yAxis, cap.Q + yAxis ) );
    intersect[ 1 ] = doesIntersect( ray, Circle( cap.P, cap.radius ) );
    
    // TODO: NOTE: This eliminates duplicate and unnecessary calculation. Probably temporary. This is only possible because collision and obstacles are very strict about consistent direction and closure.
    //intersect[ 0 ] = doesIntersect( ray, LineSeg( cap.P + yAxis, cap.Q + yAxis ) );
    //intersect[ 1 ] = doesIntersect( ray, LineSeg( cap.Q - yAxis, cap.P - yAxis ) );
    //intersect[ 2 ] = doesIntersect( ray, Circle( cap.P, cap.radius ) );
    //intersect[ 3 ] = doesIntersect( ray, Circle( cap.Q, cap.radius ) );
    
    //if( doLog ) {
    //logCollision( intersect[ 0 ] );
    //logCollision( intersect[ 1 ] );
    //logCollision( intersect[ 2 ] );
    //logCollision( intersect[ 3 ] );
    //}
    
    for( uint32 iter = 0; iter < 2; iter++ ) {
        RAY2_INTERSECT i = intersect[ iter ];
        if( ( i.isValid ) && ( i.t < result.t ) ) {
            result = i;
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
doesIntersect( RAY2 ray, orect bound, flo32 radius ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 xAxis = bound.xAxis * bound.halfDim.x;
    vec2 yAxis = bound.yAxis * bound.halfDim.y;
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = bound.center - xAxis - yAxis;
    point[ 1 ] = bound.center - xAxis + yAxis;
    point[ 2 ] = bound.center + xAxis + yAxis;
    point[ 3 ] = bound.center + xAxis - yAxis;
    point[ 4 ] = point[ 0 ];
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        CAPSULE2 cap = Capsule2( P, Q, radius );
        RAY2_INTERSECT intersect = doesIntersect( ray, cap );
        if( ( intersect.isValid ) && ( intersect.t < result.t ) ) {
            result = intersect;
        }
    }
    
    return result;
}

//----------
// ray3 functions
//----------

struct RAY3 {
    vec3 origin;
    vec3 vector;
};

struct RAY3_INTERSECT {
    boo32  isValid;
    flo32  t;
    vec3   P;
};

internal RAY3
Ray3( vec3 origin, vec3 vector ) {
    RAY3 result = {};
    result.origin = origin;
    result.vector = vector;
    return result;
}

//----------
// plane functions
//----------

struct PLANE {
    vec3  normal;
    flo32 distFromOrigin;
};

internal PLANE
Plane( vec3 normal, flo32 distFromOrigin ) {
    PLANE result = {};
    result.normal         = normal;
    result.distFromOrigin = distFromOrigin;
    return result;
}

internal RAY3_INTERSECT
doesIntersectPlane( RAY3 ray, PLANE plane ) {
    RAY3_INTERSECT result = {};
    
    flo32 denom = dot( plane.normal, ray.vector );
    if( fabsf( denom ) >= DOT__COLLINEAR_EPSILON ) {
        flo32 t = ( plane.distFromOrigin - dot( plane.normal, ray.origin ) ) / denom;
        result.isValid = true;
        result.t       = t;
        result.P       = ray.origin + ray.vector * t;
    }
    return result;
}
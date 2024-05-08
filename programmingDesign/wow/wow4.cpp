/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <cstdio>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

enum ColorType { red = 0, blue = 1 };
enum WarriorType { dragon = 0, ninja = 1, iceman = 2, lion = 3, wolf = 4 };
enum WeaponType { sword = 0, bomb = 1, arrow = 2 };
const char  KINDNAMES[ 5 ][ 10 ] = { "dragon", "ninja", "iceman", "lion", "wolf" };
const char  WEAPONS[ 3 ][ 10 ]   = { "sword", "bomb", "arrow" };
const int   RED_ORDER[ 5 ] = { 2, 3, 4, 1, 0 }, BLUE_ORDER[] = { 3, 0, 1, 2, 4 };
int         forceForAll[ 5 ];
int         hour, minute;
int         m, k;
bool        STOP;
std::string colorToString( ColorType col ) {
    if ( col == red )
        return "red";
    else
        return "blue";
}
/*****************Basic Classes**********************/
class Warrior;
class WarriorWithWeapon;
class Weapon;
class City;
class Headquarter;
/*************** headquarter ********************/
class City {
public:
    int  count;
    int  flag;
    int  health;
    int  lastWinner;
    bool fighted;

    std::shared_ptr< Warrior > warrior[ 2 ];
    std::shared_ptr< Warrior > this_time_warrior[ 2 ];
    City( int x ) : count( x ), flag( -1 ), health( 0 ), lastWinner( -1 ) {
        warrior[ 0 ] = warrior[ 1 ] = nullptr;
        this_time_warrior[ 0 ] = this_time_warrior[ 1 ] = nullptr;
    }
    virtual void walk();

    void update();
    void fight();
    void runaway();
    void report() const;
    void win( ColorType );
    void boom();
    void shot();
    void reward( ColorType );
    void getHealthForHeadquarter();
    virtual ~City();

protected:
    void reportDied( int cnt );
};

std::shared_ptr< City > Citys[ 1000 ];

class Headquarter : public City {
public:
    Headquarter( int x ) : City( x ) {}
    static int Cost[ 10 ];
    int        Order[ 5 ], Count[ 10 ];
    int        health, currentPosition, totalCount, enemyCount;
    ColorType  color;
    void       init();
    bool       available( WarriorType );
    void       Summon();
    void       report();
    bool       died();
    void       walk();
};

std::shared_ptr< Headquarter > Red, Blue;
int                            Headquarter::Cost[ 10 ] = { 0 };

/********************************/

/*************** weapon ********************/
class Weapon {
protected:
    Weapon() {}

public:
    virtual ~Weapon(){};
    virtual char* getName()   = 0;
    virtual bool  available() = 0;
    virtual int   useWeapon() = 0;
};
class Sword : public Weapon {
public:
    const char name[ 6 ] = "sword";
    bool       avail;
    int        force;
    Sword( WarriorWithWeapon* x );
    int  useWeapon();
    bool available() {
        return force > 0;
    }
    char* getName() {
        return ( char* )name;
    }
};
class Bomb : public Weapon {
public:
    const char name[ 6 ] = "bomb";
    bool       used;
    int        useWeapon();
    Bomb() {
        used = 0;
    }
    bool available() {
        return !used;
    }
    char* getName() {
        return ( char* )name;
    }
};
class Arrow : public Weapon {
public:
    const char name[ 6 ] = "arrow";
    int        count;
    static int R;
    int        useWeapon();
    Arrow() {
        count = 3;
    }
    char* getName() {
        return ( char* )name;
    }
    bool available() {
        return count > 0;
    }
};
int Arrow::R = 0;
/****************************/

/*************** warriors ********************/
class Warrior {

public:
    int       health;
    int       force;
    int       number;
    int       cityPosition;
    ColorType color;
    bool      isAlive;
    char      name[ 10 ];

protected:
    Warrior( int health_, ColorType col ) {
        this->health = health_;
        if ( col == red ) {
            number = Red->totalCount;
        }
        else {
            number = Blue->totalCount;
        }
        color        = col;
        cityPosition = col == red ? 0 : m + 1;
        isAlive      = 1;
    }

public:
    virtual void SummonPrint( const Headquarter* H ) = 0;
    virtual ~Warrior() {}
    virtual void hurt( int x, int type, WarriorWithWeapon* enemy ) {
        /***
         * func: been hurt
         * input: x: hurt;type: 0->no fight_back,1->fight_back;enemy
         ***/
        if ( !this->isAlive )
            return;
        this->health -= x;
        if ( this->health <= 0 )
            isAlive = false;
        if ( isAlive && type == 1 )
            this->fight_back( enemy );
    }
    virtual void walk();
    virtual void update()                         = 0;
    virtual void attack( WarriorWithWeapon* )     = 0;
    virtual void report()                         = 0;
    virtual void fight_back( WarriorWithWeapon* ) = 0;
    virtual bool runaway() {
        return 0;
    }
    virtual void yell( City* c ) {};
    virtual bool evaluation( WarriorWithWeapon*, bool ) = 0;
    virtual void updateMorale( double t ) {}
    virtual void rubWeapon( std::shared_ptr< WarriorWithWeapon > ){};
    virtual void shot( WarriorWithWeapon* enemy ) = 0;
};

class WarriorWithWeapon : public Warrior {
public:
    std::vector< std::shared_ptr< Weapon > > Weapons[ 3 ];

    int weaponCount;
    int current_weapon;

protected:
    WarriorWithWeapon( const int health_, ColorType color_, int type = 1, int war = 0 ) : Warrior( health_, color_ ) {
        this->force = forceForAll[ war ];
        newWeapon( Weapons, type );
        weaponCount = type;
    }

public:
    void SummonPrint( const Headquarter* H );

    void newWeapon( std::vector< std::shared_ptr< Weapon > > weapons[], const int n );
    ~WarriorWithWeapon() {
        for ( auto i : Weapons[ 0 ] )
            i.reset();
        for ( auto i : Weapons[ 1 ] )
            i.reset();
        for ( auto i : Weapons[ 2 ] )
            i.reset();
    }
    void attack( WarriorWithWeapon* enemy );
    void update();
    void report();
    void fight_back( WarriorWithWeapon* );
    bool evaluation( WarriorWithWeapon*, bool );
    void shot( WarriorWithWeapon* );
};

class Iceman : public WarriorWithWeapon {
public:
    int walk_num;
    Iceman( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ ( int )iceman ], col, 1, iceman ), walk_num( 0 ) {
        strcpy( name, "iceman" );
    }
    virtual void walk() {
        if ( walk_num == 1 && this->health > 9 )
            this->hurt( 9, 0, nullptr ), this->force += 20;
        else if ( walk_num == 1 )
            this->health = 1, this->force += 20;
        walk_num++;
        walk_num %= 2;
        WarriorWithWeapon::walk();
    }
};

class Dragon : public WarriorWithWeapon {
public:
    double morale;
    Dragon( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ dragon ], col, 1, dragon ) {
        strcpy( name, "dragon" );
        switch ( col ) {
        case red:
            morale = ( double )Red->health / ( double )Headquarter::Cost[ dragon ];
            break;
        case blue:
            morale = ( double )Blue->health / ( double )Headquarter::Cost[ dragon ];
            break;
        default:
            break;
        }
    }
    void SummonPrint( const Headquarter* H ) {
        WarriorWithWeapon::SummonPrint( H );
        std::cout << "Its morale is " << std::fixed << std::setprecision( 2 ) << morale << std::endl;
    }
    void yell( City* city ) {
        if ( this->morale > 0.8 )
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
                      << " yelled in city " << city->count << std::endl;
    }
    void updateMorale( double t ) {
        morale += t;
    }
};

class Ninja : public WarriorWithWeapon {
public:
    Ninja( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ ninja ], col, 2, ninja ) {
        strcpy( name, "ninja" );
    }
    void fight_back( WarriorWithWeapon* enemy ) {}
};

class Lion : public WarriorWithWeapon {
public:
    int loyalty;
    Lion( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ lion ], col, 0, lion ) {
        strcpy( name, "lion" );
        switch ( col ) {
        case red:
            loyalty = Red->health;
            break;
        case blue:
            loyalty = Blue->health;
            break;
        default:
            break;
        }
    }
    void SummonPrint( const Headquarter* H ) {
        WarriorWithWeapon::SummonPrint( H );
        std::cout << "Its loyalty is " << this->loyalty << std::endl;
    }
    bool runaway() {
        if ( loyalty <= 0 ) {
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " lion " << number << " ran away"
                      << std::endl;
            health = 0;
            return 1;
        }
        return 0;
    }
    void updateMorale( double x ) {
        if ( x <= 0 )
            loyalty -= k;
    }
};
class Wolf : public WarriorWithWeapon {
public:
    Wolf( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ wolf ], col, 0, wolf ) {
        strcpy( name, "wolf" );
    }
    void rubWeapon( std::shared_ptr< WarriorWithWeapon > );
};

/*********************/

/***********************Weapon Function********************/

Sword::Sword( WarriorWithWeapon* x ) {
    avail = 1;
    force = x->force * 2 / 10;
}

int Bomb::useWeapon() {
    return -1;
}
int Arrow::useWeapon() {
    if ( this->count <= 0 )
        return -1;
    this->count--;
    return R;
}
int Sword::useWeapon() {
    int ret = force;

    force = force * 8 / 10;
    if ( force == 0 )
        avail = 0;
    return ret;
}

/*****************Warrior Function****************************/

void WarriorWithWeapon::SummonPrint( const Headquarter* H ) {
    std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ":00" << " " << colorToString( color ) << " " << name << " " << H->totalCount << " born" << std::endl;
}
void Warrior::walk() {
    if ( this->color == red )
        cityPosition++;
    else
        cityPosition--;
    if ( cityPosition == 0 ) {
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
                  << " reached red headquarter with " << this->health << " elements and force " << this->force << std::endl;
    }

    else if ( cityPosition == m + 1 ) {
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
                  << " reached blue headquarter with " << this->health << " elements and force " << this->force << std::endl;
    }
    else {
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
                  << " marched to city " << cityPosition << " with " << health << " elements and force " << force << std::endl;
    }
}
void WarriorWithWeapon::newWeapon( std::vector< std::shared_ptr< Weapon > > weapon[], int n ) {
    for ( int i = 0; i < n; i++ ) {
        switch ( ( this->number + i ) % 3 ) {
        case arrow:
            weapon[ arrow ].push_back( std::shared_ptr< Weapon >( new Arrow() ) );
            break;
        case bomb:
            weapon[ bomb ].push_back( std::shared_ptr< Weapon >( new Bomb() ) );
            break;
        case sword:
            weapon[ sword ].push_back( std::shared_ptr< Weapon >( new Sword( this ) ) );
            break;
        }
    }
}

void WarriorWithWeapon::attack( WarriorWithWeapon* enemy ) {
    if ( !this->isAlive )
        return;

    int h = this->force;
    if ( !this->Weapons[ sword ].empty() )
        h += this->Weapons[ sword ].front()->useWeapon();
    std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
              << " attacked " << colorToString( enemy->color ) << " " << enemy->name << " " << enemy->number << " in city " << this->cityPosition << " with " << this->health << " elements and force "
              << this->force << std::endl;
    enemy->hurt( h, 1, this );
}

void WarriorWithWeapon::update() {
    /***
     * func: clean used weapons
     ***/
    for ( int weapontype = 0; weapontype <= 2; weapontype++ )
        for ( auto it = Weapons[ weapontype ].begin(); it != Weapons[ weapontype ].end(); ) {
            if ( !( *it )->available() ) {
                *it = nullptr;
                it  = Weapons[ weapontype ].erase( it );
                weaponCount--;
                return;
            }
            else {
                it++;
            }
        }
}

void WarriorWithWeapon::report() {
    /***
     * func: report *this current situation
     ***/
    this->update();
    bool printed = false;
    std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number << " has ";
    if ( !this->Weapons[ arrow ].empty() )
        std::cout << "arrow(" << std::dynamic_pointer_cast< Arrow >( Weapons[ arrow ].back() )->count << ")", printed = true;
    if ( !this->Weapons[ bomb ].empty() ) {
        if ( printed )
            std::cout << ",";
        std::cout << "bomb";
        printed = true;
    }
    if ( !this->Weapons[ sword ].empty() ) {
        if ( printed )
            std::cout << ",";
        std::cout << "sword(" << std::dynamic_pointer_cast< Sword >( Weapons[ sword ].back() )->force << ")", printed = true;
        printed = true;
    }
    if ( !printed )
        std::cout << "no weapon";
    std::cout << std::endl;
}

void WarriorWithWeapon::fight_back( WarriorWithWeapon* enemy ) {
    /***
     * func:fight back
     * input: who to fight back
     * ret: no ret
     ***/
    int f;
    if ( !this->Weapons[ sword ].empty() )
        f = this->force / 2 + this->Weapons[ sword ].front()->useWeapon();
    else
        f = this->force / 2;
    enemy->hurt( f, 0, this );
    std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
              << " fought back against " << colorToString( enemy->color ) << " " << enemy->name << " " << enemy->number << " in city " << this->cityPosition << std::endl;
}

bool WarriorWithWeapon::evaluation( WarriorWithWeapon* enemy, bool first ) {
    /***
     * func: evaluate whether need to boom
     * ret: need to boom -> true
     ***/
    if ( enemy->health <= 0 )
        return false;
    if ( this->health <= 0 )
        return false;
    if ( this->Weapons[ bomb ].empty() )
        return false;
    if ( first ) {
        if ( enemy->name[ 0 ] == 'n' )
            return false;
        int hurt = this->force;
        if ( !this->Weapons[ sword ].empty() )
            hurt += ( *( Sword* )( this->Weapons[ sword ].front().get() ) ).force;
        if ( enemy->health <= hurt )
            return false;
        int fight_back_hurt = enemy->force / 2;
        if ( !enemy->Weapons[ sword ].empty() )
            fight_back_hurt += ( *( Sword* )( enemy->Weapons[ sword ].front().get() ) ).force;
        return this->health <= fight_back_hurt;
    }
    else {
        int fight_back_hurt = enemy->force;
        if ( !enemy->Weapons[ sword ].empty() )
            fight_back_hurt += ( *( Sword* )( enemy->Weapons[ sword ].front().get() ) ).force;
        return this->health <= fight_back_hurt;
    }
}

void WarriorWithWeapon::shot( WarriorWithWeapon* enemy ) {
    if ( !this->Weapons[ arrow ].empty() ) {
        if ( !this->Weapons[ arrow ][ 0 ]->available() )
            Weapons[ arrow ][ 0 ] = nullptr, Weapons[ arrow ].pop_back();
        else {
            enemy->health -= Weapons[ arrow ][ 0 ]->useWeapon();
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( color ) << " " << name << " " << number
                      << " shot";
            if ( enemy->health <= 0 )
                std::cout << " and killed " << colorToString( enemy->color ) << " " << enemy->name << " " << enemy->number;
            std::cout << std::endl;
        }
    }
}

void Wolf::rubWeapon( std::shared_ptr< WarriorWithWeapon > target ) {
    if ( target == nullptr )
        return;
    if ( this->Weapons[ sword ].empty() && !target->Weapons[ sword ].empty() )
        this->Weapons[ sword ].push_back( target->Weapons[ sword ].back() );
    if ( this->Weapons[ arrow ].empty() && !target->Weapons[ arrow ].empty() )
        this->Weapons[ arrow ].push_back( target->Weapons[ arrow ].back() );
    if ( this->Weapons[ bomb ].empty() && !target->Weapons[ bomb ].empty() )
        this->Weapons[ bomb ].push_back( target->Weapons[ bomb ].back() );
}

/*************************headquarter function****************************/

City::~City() {
    if ( warrior[ 0 ] != nullptr )
        warrior[ 0 ].reset();
    if ( warrior[ 1 ] != nullptr )
        warrior[ 1 ].reset();
    warrior[ 0 ] = nullptr;
    warrior[ 1 ] = nullptr;
}
bool Headquarter::available( WarriorType wa ) {
    return Cost[ wa ] <= this->health;
}

bool Headquarter::died() {
    if ( warrior[ 1 - this->color ] != nullptr )
        enemyCount++, warrior[ 1 - this->color ] = nullptr;
    return enemyCount >= 2;
}

void Headquarter::report() {
    std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << this->health << " elements in " << colorToString( color ) << " "
              << "headquarter" << std::endl;
}

void Headquarter::Summon() {
    currentPosition++;
    currentPosition %= 5;
    WarriorType this_warrior = ( WarriorType )Order[ currentPosition ];
    if ( available( this_warrior ) ) {
        totalCount++;
        health -= Cost[ this_warrior ];
        Count[ this_warrior ]++;
        switch ( Order[ currentPosition ] ) {
        case iceman:
            warrior[ this->color ].reset( new Iceman( this->color ) );
            break;

        case wolf:
            warrior[ this->color ].reset( new Wolf( this->color ) );
            break;

        case ninja:
            warrior[ this->color ].reset( new Ninja( this->color ) );
            break;
        case dragon:
            warrior[ this->color ].reset( new Dragon( this->color ) );
            break;
        case lion:
            warrior[ this->color ].reset( new Lion( this->color ) );
            break;
        default:
            break;
        }
        warrior[ this->color ]->SummonPrint( this );
        flag = 1;
    }
    else
        currentPosition--, currentPosition = ( currentPosition + 5 ) % 5;
}

void Headquarter::init() {
    warrior[ 0 ] = warrior[ 1 ] = nullptr;
    for ( int i = 0; i <= 9; i++ )
        Cost[ i ] = 0, Count[ i ] = 0;
    health          = 0;
    currentPosition = -1;
    totalCount      = 0;
    enemyCount      = 0;
}

void City::walk() {
    if ( count - 1 >= 0 ) {
        if ( Citys[ this->count - 1 ]->warrior[ 0 ] != nullptr ) {
            Citys[ this->count - 1 ]->warrior[ 0 ]->walk();
            this->this_time_warrior[ 0 ] = Citys[ this->count - 1 ]->warrior[ 0 ];
        }
        else
            this->this_time_warrior[ 0 ] = nullptr;
    }
    if ( count <= m ) {
        if ( Citys[ this->count + 1 ]->warrior[ 1 ] != nullptr ) {
            Citys[ this->count + 1 ]->warrior[ 1 ]->walk();
            this->this_time_warrior[ 1 ] = Citys[ this->count + 1 ]->warrior[ 1 ];
        }
        else
            this->this_time_warrior[ 1 ] = nullptr;
    }
}

void Headquarter::walk() {
    if ( count - 1 >= 0 ) {
        if ( Citys[ this->count - 1 ]->warrior[ 0 ] != nullptr ) {
            Citys[ this->count - 1 ]->warrior[ 0 ]->walk();
            this->this_time_warrior[ 0 ] = Citys[ this->count - 1 ]->warrior[ 0 ];
            enemyCount++;
            if ( enemyCount >= 2 ) {
                STOP = 1;
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " blue headquarter was taken" << std::endl;
                return;
            }
        }
    }
    if ( count <= m ) {
        if ( Citys[ this->count + 1 ]->warrior[ 1 ] != nullptr ) {
            Citys[ this->count + 1 ]->warrior[ 1 ]->walk();
            this->this_time_warrior[ 1 ] = Citys[ this->count + 1 ]->warrior[ 1 ];
            enemyCount++;
            if ( enemyCount >= 2 ) {
                STOP = 1;
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " red headquarter was taken" << std::endl;
                return;
            }
        }
    }
}

void City::update() {
    this->fighted      = 0;
    this->warrior[ 0 ] = this->this_time_warrior[ 0 ];
    this->warrior[ 1 ] = this->this_time_warrior[ 1 ];
}

void City::fight() {
    this->update();
    if ( this->warrior[ 0 ] == nullptr || this->warrior[ 1 ] == nullptr ) {
        if ( this->warrior[ 0 ] == nullptr && this->warrior[ 1 ] != nullptr && this->warrior[ 1 ]->health <= 0 )
            this->warrior[ 1 ] = nullptr;
        if ( this->warrior[ 1 ] == nullptr && this->warrior[ 0 ] != nullptr && this->warrior[ 0 ]->health <= 0 )
            this->warrior[ 0 ] = nullptr;
        return;
    }
    this->fighted                 = 1;
    auto died                     = []( std::shared_ptr< Warrior > wa ) -> bool { return wa->health <= 0; };
    int  first_fight              = ( flag != -1 ? flag : ( count + 1 ) % 2 );
    int  health_before_fight[ 2 ] = { std::max( warrior[ 0 ]->health, 0 ), std::max( warrior[ 1 ]->health, 0 ) };
    if ( died( warrior[ 0 ] ) && died( warrior[ 1 ] ) ) {
        warrior[ 0 ] = nullptr;
        warrior[ 1 ] = nullptr;
        return;
    }
    if ( died( warrior[ 0 ] ) ) {
        warrior[ 0 ]->update();
        warrior[ 1 ]->update();
        warrior[ 1 ]->rubWeapon( std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ 0 ] ) );
        if ( 1 == first_fight )
            warrior[ 1 ]->updateMorale( 0.2 ), warrior[ 1 ]->yell( this );
        warrior[ 0 ] = nullptr;
        this->getHealthForHeadquarter();
        this->win( blue );
        return;
    }
    if ( died( warrior[ 1 ] ) ) {
        warrior[ 1 ]->update();
        warrior[ 0 ]->update();
        warrior[ 0 ]->rubWeapon( std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ 1 ] ) );
        if ( 0 == first_fight )
            warrior[ 0 ]->updateMorale( 0.2 ), warrior[ 0 ]->yell( this );
        warrior[ 1 ] = nullptr;
        this->getHealthForHeadquarter();
        this->win( red );
        return;
    }
    warrior[ first_fight ]->attack( ( WarriorWithWeapon* )warrior[ ( first_fight + 1 ) % 2 ].get() );
    if ( died( warrior[ 1 ] ) && died( warrior[ 0 ] ) ) {
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " both red " << warrior[ 0 ]->name << " " << warrior[ 0 ]->number
                  << " and blue " << warrior[ 1 ]->name << " " << warrior[ 1 ]->number << " died in city " << this->count << std::endl;
        warrior[ 0 ] = warrior[ 1 ] = nullptr;
        return;
    }
    else if ( died( warrior[ 1 ] ) ) {
        reportDied( 1 );
        if ( warrior[ 1 ]->name[ 0 ] == 'l' )
            warrior[ 0 ]->health += health_before_fight[ 1 ];
        if ( 0 == first_fight )
            warrior[ 0 ]->updateMorale( 0.2 ), warrior[ 0 ]->yell( this );
        warrior[ 1 ] = nullptr;
        this->getHealthForHeadquarter();
        this->win( red );
        return;
    }
    else if ( died( warrior[ 0 ] ) ) {
        reportDied( 0 );
        if ( warrior[ 0 ]->name[ 0 ] == 'l' )
            warrior[ 1 ]->health += health_before_fight[ 0 ];
        if ( 1 == first_fight )
            warrior[ 1 ]->updateMorale( 0.2 ), warrior[ 1 ]->yell( this );
        warrior[ 0 ] = nullptr;
        this->getHealthForHeadquarter();
        this->win( blue );
        return;
    }
    else {
        warrior[ 0 ]->updateMorale( -0.2 );
        warrior[ 1 ]->updateMorale( -0.2 );
        warrior[ first_fight ]->yell( this );
        this->lastWinner = -1;
        return;
    }
}

void City::runaway() {
    if ( this->warrior[ 0 ] != nullptr && this->warrior[ 0 ]->runaway() )
        this->warrior[ 0 ] = nullptr;
    if ( this->warrior[ 1 ] != nullptr && this->warrior[ 1 ]->runaway() )
        this->warrior[ 1 ] = nullptr;
}

void City::report() const {
    if ( this->warrior[ 0 ] != nullptr )
        this->warrior[ 0 ]->report();
    if ( this->warrior[ 1 ] != nullptr )
        this->warrior[ 1 ]->report();
}

void City::win( ColorType col ) {
    if ( this->lastWinner == col && this->lastWinner != -1 ) {
        if ( flag != col )
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( col ) << " " << "flag raised in city "
                      << this->count << std::endl;
        flag = col;
    }
    else
        lastWinner = col;
}

void City::reportDied( int count ) {
    std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( warrior[ count % 2 ]->color ) << " "
              << warrior[ count % 2 ]->name << " " << warrior[ count % 2 ]->number << " was killed in city " << this->count << std::endl;
    warrior[ count % 2 ]->update();
    warrior[ ( count + 1 ) % 2 ]->update();
    warrior[ ( count + 1 ) % 2 ]->rubWeapon( std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ count % 2 ] ) );
}

void City::boom() {
    int first_fight = ( flag != -1 ? flag : ( count + 1 ) % 2 );
    int next_fight  = 1 - first_fight;

    if ( warrior[ 0 ] == nullptr || warrior[ 1 ] == nullptr )
        return;

    if ( warrior[ first_fight ]->evaluation( ( WarriorWithWeapon* )warrior[ next_fight ].get(), 1 ) ) {
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( warrior[ first_fight ]->color ) << " "
                  << warrior[ first_fight ]->name << " " << warrior[ first_fight ]->number << " used a bomb and killed " << colorToString( warrior[ next_fight ]->color ) << " "
                  << warrior[ next_fight ]->name << " " << warrior[ next_fight ]->number << std::endl;
        warrior[ 0 ]->health = 0, warrior[ 1 ]->health = 0;
        warrior[ 0 ]->isAlive = false, warrior[ 1 ]->isAlive = false;
        warrior[ 0 ] = nullptr, warrior[ 1 ] = nullptr;
    }
    else if ( warrior[ next_fight ]->evaluation( ( WarriorWithWeapon* )warrior[ first_fight ].get(), 0 ) ) {
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( warrior[ next_fight ]->color ) << " "
                  << warrior[ next_fight ]->name << " " << warrior[ next_fight ]->number << " used a bomb and killed " << colorToString( warrior[ first_fight ]->color ) << " "
                  << warrior[ first_fight ]->name << " " << warrior[ first_fight ]->number << std::endl;
        warrior[ 0 ]->health = 0, warrior[ 1 ]->health = 0;
        warrior[ 0 ]->isAlive = false, warrior[ 1 ]->isAlive = false;
        warrior[ 0 ] = nullptr, warrior[ 1 ] = nullptr;
    }
}

void City::shot() {
    if ( this->warrior[ 0 ] != nullptr && this->count + 1 <= m && Citys[ this->count + 1 ]->warrior[ 1 ] != nullptr )
        this->warrior[ 0 ]->shot( ( WarriorWithWeapon* )Citys[ this->count + 1 ]->warrior[ 1 ].get() );
    if ( this->warrior[ 1 ] != nullptr && this->count - 1 >= 1 && Citys[ this->count - 1 ]->warrior[ 0 ] != nullptr )
        this->warrior[ 1 ]->shot( ( WarriorWithWeapon* )Citys[ this->count - 1 ]->warrior[ 0 ].get() );
}

void City::reward( ColorType col ) {
    if ( this->warrior[ col ] != nullptr && this->warrior[ 1 - col ] == nullptr )
        if ( std::dynamic_pointer_cast< Headquarter >( Citys[ col * m ] )->health >= 8 )
            this->warrior[ col ]->health += 8, std::dynamic_pointer_cast< Headquarter >( Citys[ col * m ] )->health -= 8;
}

void City::getHealthForHeadquarter() {
    ColorType col = red;
    if ( this->warrior[ col ] != nullptr && this->warrior[ 1 - col ] == nullptr ) {
        Red->health += this->health;
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( col ) << " " << this->warrior[ col ]->name << " "
                  << this->warrior[ col ]->number << " earned " << this->health << " elements for his headquarter" << std::endl;
        this->health = 0;
    }
    col = blue;
    if ( this->warrior[ col ] != nullptr && this->warrior[ 1 - col ] == nullptr ) {
        Blue->health += this->health;
        // if ( this->fighted )
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << colorToString( col ) << " " << this->warrior[ col ]->name << " "
                  << this->warrior[ col ]->number << " earned " << this->health << " elements for his headquarter" << std::endl;
        this->health = 0;
    }
}

int main() {
    int testCase;
#ifdef llt
    freopen( "./Warcraft.in", "r", stdin );
    freopen( "./a.out", "w", stdout );
#endif
    std::cin >> testCase;
    int currentTestCase = 0;
    while ( currentTestCase < testCase ) {
        int health;
        int T, t = -5;
        STOP = 0;
        std::cin >> health >> m >> Arrow::R >> k >> T;
        Red.reset( new Headquarter( 0 ) ), Blue.reset( new Headquarter( m + 1 ) );
        Red->Order[ 0 ] = 2, Red->Order[ 1 ] = 3, Red->Order[ 2 ] = 4, Red->Order[ 3 ] = 1, Red->Order[ 4 ] = 0;
        Blue->Order[ 0 ] = 3, Blue->Order[ 1 ] = 0, Blue->Order[ 2 ] = 1, Blue->Order[ 3 ] = 2, Blue->Order[ 4 ] = 4;
        Red->color = red, Blue->color = blue;
        currentTestCase++;
        hour = -1;
        Red->init(), Blue->init();
        Blue->health = Red->health = health;

        Citys[ 0 ]     = Red;
        Citys[ m + 1 ] = Blue;
        for ( int i = 1; i <= m; i++ )
            Citys[ i ].reset( new City( i ) );
        Citys[ 0 ]     = Red;
        Citys[ m + 1 ] = Blue;
        std::cin >> Headquarter::Cost[ 0 ] >> Headquarter::Cost[ 1 ] >> Headquarter::Cost[ 2 ] >> Headquarter::Cost[ 3 ] >> Headquarter::Cost[ 4 ];
        std::cin >> forceForAll[ 0 ] >> forceForAll[ 1 ] >> forceForAll[ 2 ] >> forceForAll[ 3 ] >> forceForAll[ 4 ];
        std::cout << "Case " << currentTestCase << ":" << std::endl;
        while ( true ) {
            {
                // xxx:00
                t += 5;
                if ( t > T )
                    break;
                minute = 0;
                hour++;
                Red->Summon();
                Blue->Summon();
            }

            {
                // xxx:05
                t += 5;
                if ( t > T )
                    break;
                minute = 5;
                for ( int i = 0; i <= m + 1; i++ )
                    Citys[ i ]->runaway();
            }

            {
                // xxx:10
                t += 5;
                if ( t > T )
                    break;
                minute = 10;
                for ( int i = 0; i <= m + 1; i++ )
                    Citys[ i ]->walk();
            }
            if ( STOP )
                break;
            for ( int i = 0; i <= m + 1; i++ )
                Citys[ i ]->update();
            {
                // xxx:20
                t += 10;
                if ( t > T )
                    break;
                minute = 20;
                for ( int i = 1; i <= m; i++ )
                    Citys[ i ]->health += 10;
            }

            {
                // xxx:30
                t += 10;
                if ( t > T )
                    break;
                minute = 30;
                for ( int i = 1; i <= m; i++ )
                    Citys[ i ]->getHealthForHeadquarter();
            }
            {
                // xxx:35
                t += 5;
                if ( t > T )
                    break;
                minute = 35;
                for ( int i = 1; i <= m; i++ )
                    Citys[ i ]->shot();
            }
            {
                // xxx:38
                t += 3;
                if ( t > T )
                    break;
                minute = 38;
                for ( int i = 1; i <= m; i++ )
                    Citys[ i ]->boom();
            }
            {
                // xxx:40
                t += 2;
                if ( t > T )
                    break;
                minute = 40;
                for ( int i = 1; i <= m; i++ )
                    Citys[ i ]->fight();
            }
            {
                // reward

                auto win = [ = ]( int i, ColorType col ) { return Citys[ i ]->fighted && Citys[ i ]->warrior[ col ] != nullptr && Citys[ i ]->warrior[ 1 - col ] == nullptr; };
                for ( int i = 1; i <= m; i++ )  // blue
                    if ( win( i, blue ) && Blue->health >= 8 )
                        Citys[ i ]->warrior[ blue ]->health += 8, Blue->health -= 8;
                for ( int i = m; i >= 1; i-- )
                    if ( win( i, red ) && Red->health >= 8 )
                        Citys[ i ]->warrior[ red ]->health += 8, Red->health -= 8;
            }
            {
                // xx:50
                t += 10;
                if ( t > T )
                    break;
                minute = 50;
                Red->report();
                Blue->report();
            }
            {
                t += 5;
                if ( t > T )
                    break;
                minute = 55;
                for ( int i = 0; i <= m + 1; i++ )
                    if ( Citys[ i ]->warrior[ red ] != nullptr )
                        Citys[ i ]->warrior[ red ]->report();
                for ( int i = 0; i <= m + 1; i++ )
                    if ( Citys[ i ]->warrior[ blue ] != nullptr )
                        Citys[ i ]->warrior[ blue ]->report();
            }
        }
    }
}

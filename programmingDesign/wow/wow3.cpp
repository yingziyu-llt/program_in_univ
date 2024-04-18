/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

enum ColorType { red = 0, blue = 1 };
enum WarriorType { dragon = 0, ninja = 1, iceman = 2, lion = 3, wolf = 4 };
enum WeaponType { sword = 0, bomb = 1, arrow = 2 };
const char KINDNAMES[ 5 ][ 10 ] = { "dragon", "ninja", "iceman", "lion", "wolf" };
const char WEAPONS[ 3 ][ 10 ]   = { "sword", "bomb", "arrow" };
const int  RED_ORDER[ 5 ] = { 2, 3, 4, 1, 0 }, BLUE_ORDER[] = { 3, 0, 1, 2, 4 };
int        forceForAll[ 5 ];
int        hour, minute;
bool       disableOutput = true;
int        m, k;
/*****************Basic Classes**********************/
class Warrior;
class Weapon;
class City;
class Headquarter;
/*************** headquarter ********************/
class City {
public:
    int                        count;
    std::shared_ptr< Warrior > warrior[ 2 ];
    std::shared_ptr< Warrior > this_time_warrior[ 2 ];
    City( int x ) : count( x ) {
        warrior[ 0 ] = warrior[ 1 ] = nullptr;
        this_time_warrior[ 0 ] = this_time_warrior[ 1 ] = nullptr;
    }
    virtual void walk();

    void update();
    void fight();
    void runaway();
    void rub();
    void report() const;
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
    int        health, currentPosition, totalCount;
    bool       ableToSummon;
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
    virtual char* getName()                     = 0;
    virtual bool  available()                   = 0;
    virtual int   useWeapon( Warrior* warrior ) = 0;
};
class Sword : public Weapon {
public:
    const char name[ 6 ] = "sword";
    double     hurtFactor;
    Sword() {
        hurtFactor = 1;
    }
    int  useWeapon( Warrior* warrior );
    bool available() {
        return true;
    }
    char* getName() {
        return ( char* )name;
    }
};
class Bomb : public Weapon {
public:
    const char name[ 6 ] = "bomb";
    bool       used;
    int        useWeapon( Warrior* warrior );
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
    int        useWeapon( Warrior* warrior );
    Arrow() {
        count = 2;
    }
    char* getName() {
        return ( char* )name;
    }
    bool available() {
        return count > 0;
    }
};
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
    virtual void hurt( int x, int type ) {
        this->health -= x;
    }
    virtual void walk();
    virtual void update()      = 0;
    virtual void totalUpdate() = 0;
    virtual int  attack()      = 0;
    virtual void report()      = 0;
    virtual bool runaway() {
        return 0;
    }
    virtual void yell( City* c ) {};
};

class WarriorWithWeapon : public Warrior {
public:
    std::vector< std::shared_ptr< Weapon > > Weapons[ 3 ];
    std::vector< std::shared_ptr< Weapon > > CurrentWeapons;

    int weaponCount;
    int current_weapon;

protected:
    WarriorWithWeapon( const int health_, ColorType color_, int type = 1 ) : Warrior( health_, color_ ) {
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
    int  attack();
    void update();
    void totalUpdate();
    void report();

    virtual void rub( std::shared_ptr< WarriorWithWeapon > );
    virtual void rubWeapon( std::shared_ptr< WarriorWithWeapon > ) {}
};

class Iceman : public WarriorWithWeapon {
public:
    Iceman( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ ( int )iceman ], col ) {
        force = forceForAll[ iceman ];
        strcpy( name, "iceman" );
    }
    virtual void walk() {
        health -= health / 10;
        WarriorWithWeapon::walk();
    }
};

class Dragon : public WarriorWithWeapon {
public:
    double morale;
    Dragon( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ dragon ], col ) {
        strcpy( name, "dragon" );
        force = forceForAll[ dragon ];
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
    void yell( City* city ) {
        if ( !disableOutput )
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                      << number << " yelled in city " << city->count << std::endl;
    }
};

class Ninja : public WarriorWithWeapon {
public:
    Ninja( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ ninja ], col, 2 ) {
        strcpy( name, "ninja" );
        force = forceForAll[ ninja ];
    }
    int attack();
};

class Lion : public WarriorWithWeapon {
public:
    int loyalty;
    Lion( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ lion ], col, 1 ) {
        strcpy( name, "lion" );
        force = forceForAll[ lion ];
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
        if ( !disableOutput )
            std::cout << "Its loyalty is " << this->loyalty << std::endl;
    }
    virtual void walk() {
        loyalty -= k;
        if ( this->color == red )
            cityPosition++;
        else
            cityPosition--;
        if ( cityPosition == 0 ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                          << number << " reached red headquarter with " << this->health << " elements and force " << this->force << std::endl;
        }
        else if ( cityPosition == m + 1 ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                          << number << " reached blue headquarter with " << this->health << " elements and force " << this->force << std::endl;
        }
        else {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                          << number << " marched to city " << cityPosition << " with " << health << " elements and force " << force << std::endl;
        }
    }
    bool runaway() {
        if ( loyalty <= 0 ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( this->color == red ? "red" : "blue" ) << " lion "
                          << number << " ran away" << std::endl;
            health = 0;
            return 1;
        }
        return 0;
    }
};
class Wolf : public WarriorWithWeapon {
public:
    Wolf( ColorType col ) : WarriorWithWeapon( Headquarter::Cost[ wolf ], col, 0 ) {
        strcpy( name, "wolf" );
        force = forceForAll[ wolf ];
    }
    void rubWeapon( std::shared_ptr< WarriorWithWeapon > );
};

/*********************/

/***********************Weapon Function********************/

int Bomb::useWeapon( Warrior* warrior ) {
    if ( this->used )
        return -1;
    this->used = 1;
    return warrior->force * 4 / 10;
}
int Arrow::useWeapon( Warrior* warrior ) {
    if ( this->count <= 0 )
        return -1;
    this->count--;
    return warrior->force * 3 / 10;
}
int Sword::useWeapon( Warrior* warrior ) {
    return warrior->force * 2 / 10;
}

/*****************Warrior Function****************************/

void WarriorWithWeapon::SummonPrint( const Headquarter* H ) {
    if ( !disableOutput )
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ":00" << " " << ( this->color == 0 ? "red" : "blue" ) << " " << name << " " << H->totalCount
                  << " born" /* with strength " << this->health << ","
  << H->Count[wa] << " " << name << " "
  << "in " << (col == 0 ? "red" : "blue") << " headquarter"*/
                  << std::endl;
}
void Warrior::walk() {
    if ( this->color == red )
        cityPosition++;
    else
        cityPosition--;
    if ( cityPosition == 0 ) {
        if ( !disableOutput )
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                      << number << " reached red headquarter with " << this->health << " elements and force " << this->force << std::endl;
    }

    else if ( cityPosition == m + 1 ) {
        if ( !disableOutput )
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                      << number << " reached blue headquarter with " << this->health << " elements and force " << this->force << std::endl;
    }
    else {
        if ( !disableOutput )
            std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                      << number << " marched to city " << cityPosition << " with " << health << " elements and force " << force << std::endl;
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
            weapon[ sword ].push_back( std::shared_ptr< Weapon >( new Sword() ) );
            break;
        }
    }
}

int WarriorWithWeapon::attack() {
    int cnt = 0;
    if ( weaponCount == 0 )
        return -1;
    current_weapon %= weaponCount;
    while ( weaponCount ) {
        if ( CurrentWeapons[ current_weapon ] != nullptr && !CurrentWeapons[ current_weapon ]->available() ) {
            cnt++;
            current_weapon++;
            current_weapon %= weaponCount;
            if ( cnt >= weaponCount )
                return -1;
        }
        else if ( CurrentWeapons[ current_weapon ] != nullptr && CurrentWeapons[ current_weapon ]->available() )
            break;
    }
    if ( current_weapon >= weaponCount )
        return -1;
    int attack = CurrentWeapons[ current_weapon ]->useWeapon( this );
    if ( CurrentWeapons[ current_weapon ]->getName()[ 0 ] == 'b' )
        this->hurt( attack / 2, 1 );
    current_weapon++;
    return attack;
}

void WarriorWithWeapon::update() {
    for ( int weapontype = 0; weapontype <= 2; weapontype++ )
        for ( auto it = Weapons[ weapontype ].begin(); it != Weapons[ weapontype ].end(); ) {
            if ( !( *it )->available() ) {
                *it = nullptr;
                it  = Weapons[ weapontype ].erase( it );
                weaponCount--;
            }
            else {
                it++;
            }
        }
}

void WarriorWithWeapon::totalUpdate() {
    this->update();
    CurrentWeapons.clear();
    std::copy( Weapons[ 0 ].rbegin(), Weapons[ 0 ].rend(), std::back_inserter( CurrentWeapons ) );
    std::copy( Weapons[ 1 ].rbegin(), Weapons[ 1 ].rend(), std::back_inserter( CurrentWeapons ) );
    std::copy( Weapons[ 2 ].rbegin(), Weapons[ 2 ].rend(), std::back_inserter( CurrentWeapons ) );
    this->current_weapon = 0;
}

void WarriorWithWeapon::report() {
    if ( !disableOutput )
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " " << number
                  << " has " << Weapons[ 0 ].size() << " sword " << Weapons[ 1 ].size() << " bomb " << Weapons[ 2 ].size() << " arrow and " << health << " elements" << std::endl;
}

int Ninja::attack() {
    int cnt = 0;
    if ( weaponCount == 0 )
        return -1;
    current_weapon %= weaponCount;
    while ( weaponCount ) {
        if ( CurrentWeapons[ current_weapon ] != nullptr && !CurrentWeapons[ current_weapon ]->available() ) {
            cnt++;
            current_weapon++;
            current_weapon %= weaponCount;
            if ( cnt >= weaponCount )
                return -1;
        }
        else if ( CurrentWeapons[ current_weapon ] != nullptr && CurrentWeapons[ current_weapon ]->available() )
            break;
    }
    if ( current_weapon >= weaponCount )
        return -1;
    int attack = CurrentWeapons[ current_weapon ]->useWeapon( this );
    current_weapon++;
    return attack;
}

void WarriorWithWeapon::rub( std::shared_ptr< WarriorWithWeapon > target )  // 有问题吗
{
    if ( target == nullptr )
        return;
    int cnt = 0;
    for ( int weapontype = 0; weapontype <= 2; weapontype++ ) {
        while ( !target->Weapons[ weapontype ].empty() ) {
            if ( this->weaponCount < 10 )
                this->Weapons[ weapontype ].push_back( target->Weapons[ weapontype ][ 0 ] ), target->Weapons[ weapontype ].erase( target->Weapons[ weapontype ].begin() ), this->weaponCount++,
                    target->weaponCount--;
            else
                break;
            cnt++;
        }
    }
}
void Wolf::rubWeapon( std::shared_ptr< WarriorWithWeapon > target )  // 有问题吗
{
    if ( target == nullptr )
        return;
    if ( strcmp( target->name, "wolf" ) == 0 )
        return;
    int cnt = 0;
    for ( int weapontype = 0; weapontype <= 2; weapontype++ ) {
        while ( !target->Weapons[ weapontype ].empty() ) {
            if ( this->weaponCount < 10 )
                this->Weapons[ weapontype ].push_back( target->Weapons[ weapontype ][ 0 ] ), target->Weapons[ weapontype ].erase( target->Weapons[ weapontype ].begin() ), this->weaponCount++,
                    target->weaponCount--;
            else
                break;
            cnt++;
        }
        if ( cnt ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( color == red ? "red" : "blue" ) << " " << name << " "
                          << number << " took " << cnt << " " << Weapons[ weapontype ][ 0 ]->getName() << " from " << ( target->color == red ? "red" : "blue" ) << " " << target->name << " "
                          << target->number << " in city " << this->cityPosition << std::endl;
            return;
        }
    }
}

City::~City() {
    if ( warrior[ 0 ] != nullptr )
        warrior[ 0 ].reset();
    if ( warrior[ 1 ] != nullptr )
        warrior[ 1 ].reset();
    warrior[ 0 ] = nullptr;
    warrior[ 1 ] = nullptr;
}
/*************************headquarter
 * function*****************************************/
bool Headquarter::available( WarriorType wa ) {
    return Cost[ wa ] <= this->health;
}
bool Headquarter::died() {
    switch ( color ) {
    case red:
        return warrior[ blue ] != nullptr;
    case blue:
        return warrior[ red ] != nullptr;
    }
}
void Headquarter::report() {
    if ( !disableOutput )
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << this->health << " elements in "
                  << ( color == red ? "red" : "blue" ) << " " << "headquarter" << std::endl;
}
void Headquarter::Summon() {
    if ( !ableToSummon )
        return;
    bool flag = false;
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
    if ( !flag )
        ableToSummon = false;
}
void Headquarter::init() {
    warrior[ 0 ] = warrior[ 1 ] = nullptr;
    for ( int i = 0; i <= 9; i++ )
        Cost[ i ] = 0, Count[ i ] = 0;
    health          = 0;
    ableToSummon    = 1;
    currentPosition = -1;
    totalCount      = 0;
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
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << "blue headquarter was taken" << std::endl;
        }
        else
            this->this_time_warrior[ 0 ] = nullptr;
    }
    if ( count <= m ) {
        if ( Citys[ this->count + 1 ]->warrior[ 1 ] != nullptr ) {
            Citys[ this->count + 1 ]->warrior[ 1 ]->walk();
            this->this_time_warrior[ 1 ] = Citys[ this->count + 1 ]->warrior[ 1 ];
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << "red headquarter was taken" << std::endl;
        }
        else
            this->this_time_warrior[ 1 ] = nullptr;
    }
}
void City::update() {
    this->warrior[ 0 ] = this->this_time_warrior[ 0 ];
    this->warrior[ 1 ] = this->this_time_warrior[ 1 ];
}

void City::runaway() {
    if ( this->warrior[ 0 ] != nullptr && this->warrior[ 0 ]->runaway() )
        this->warrior[ 0 ] = nullptr;
    if ( this->warrior[ 1 ] != nullptr && this->warrior[ 1 ]->runaway() )
        this->warrior[ 1 ] = nullptr;
}

void City::rub() {
    if ( this->warrior[ 0 ] != nullptr )
        std::static_pointer_cast< WarriorWithWeapon >( this->warrior[ 0 ] )->rubWeapon( std::static_pointer_cast< WarriorWithWeapon >( this->warrior[ 1 ] ) );
    if ( this->warrior[ 1 ] != nullptr )
        std::static_pointer_cast< WarriorWithWeapon >( this->warrior[ 1 ] )->rubWeapon( std::static_pointer_cast< WarriorWithWeapon >( this->warrior[ 0 ] ) );
}

void City::report() const {
    if ( this->warrior[ 0 ] != nullptr )
        this->warrior[ 0 ]->report();
    if ( this->warrior[ 1 ] != nullptr )
        this->warrior[ 1 ]->report();
}

void City::reportDied( int count ) {
    if ( !disableOutput )
        std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " " << ( warrior[ ( count + 1 ) % 2 ]->color == red ? "red" : "blue" )
                  << " " << warrior[ ( count + 1 ) % 2 ]->name << " " << warrior[ ( count + 1 ) % 2 ]->number << " killed " << ( warrior[ count % 2 ]->color == red ? "red" : "blue" ) << " "
                  << warrior[ count % 2 ]->name << " " << warrior[ count % 2 ]->number << " in city " << this->count << " remaining " << warrior[ ( count + 1 ) % 2 ]->health << " elements"
                  << std::endl;
    warrior[ count % 2 ]->update();
    warrior[ ( count + 1 ) % 2 ]->update();
    std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ ( count + 1 ) % 2 ] )->rub( std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ count % 2 ] ) );
    warrior[ count % 2 ] = nullptr;
    warrior[ ( count + 1 ) % 2 ]->yell( this );
}

void City::fight() {
    if ( this->warrior[ 0 ] == nullptr || this->warrior[ 1 ] == nullptr )
        return;
    auto died = []( std::shared_ptr< Warrior > wa ) -> bool { return wa->health <= 0; };
    warrior[ 0 ]->totalUpdate(), warrior[ 1 ]->totalUpdate();
    while ( 1 ) {
        int hurt1 = warrior[ ( count + 1 ) % 2 ]->attack();
        if ( hurt1 != -1 )
            warrior[ this->count % 2 ]->hurt( hurt1, 0 );
        if ( died( warrior[ ( count + 1 ) % 2 ] ) && died( warrior[ count % 2 ] ) ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " both red " << warrior[ 0 ]->name << " "
                          << warrior[ 0 ]->number << " and blue " << warrior[ 1 ]->name << " " << warrior[ 1 ]->number << " died in city " << this->count << std::endl;
            warrior[ 0 ] = warrior[ 1 ] = nullptr;
            break;
        }
        else if ( died( warrior[ count % 2 ] ) ) {
            reportDied( count % 2 );
            break;
        }
        else if ( died( warrior[ ( count + 1 ) % 2 ] ) ) {
            reportDied( ( count + 1 ) % 2 );
            break;
        }
        int hurt2 = warrior[ count % 2 ]->attack();
        if ( hurt2 != -1 )
            warrior[ ( this->count + 1 ) % 2 ]->hurt( hurt2, 0 );
        if ( died( warrior[ count % 2 ] ) && died( warrior[ ( count + 1 ) % 2 ] ) ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " both red " << warrior[ 0 ]->name << " "
                          << warrior[ 0 ]->number << " and blue " << warrior[ 1 ]->name << " " << warrior[ 1 ]->number << " died in city " << this->count << std::endl;
            warrior[ 0 ] = warrior[ 1 ] = nullptr;
            break;
        }
        else if ( died( warrior[ ( count + 1 ) % 2 ] ) ) {
            reportDied( ( count + 1 ) % 2 );
            break;
        }
        else if ( died( warrior[ count % 2 ] ) ) {
            reportDied( count % 2 );
            break;
        }
        if ( hurt1 == -1 && hurt2 == -1 ) {
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " both red " << warrior[ 0 ]->name << " "
                          << warrior[ 0 ]->number << " and blue " << warrior[ 1 ]->name << " " << warrior[ 1 ]->number << " were alive in city " << this->count << std::endl;
            if ( warrior[ 0 ]->name[ 0 ] == 'd' )
                warrior[ 0 ]->yell( this );
            if ( warrior[ 1 ]->name[ 0 ] == 'd' )
                warrior[ 1 ]->yell( this );
            break;
        }
        if ( hurt1 <= 0 && hurt2 <= 0 ) {
            std::shared_ptr< WarriorWithWeapon > wa0 = std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ 0 ] );
            std::shared_ptr< WarriorWithWeapon > wa1 = std::dynamic_pointer_cast< WarriorWithWeapon >( warrior[ 1 ] );
            wa0->weaponCount -= wa0->Weapons[ bomb ].size();
            wa0->weaponCount -= wa0->Weapons[ arrow ].size();
            wa0->Weapons[ bomb ].clear();
            wa0->Weapons[ arrow ].clear();
            wa1->weaponCount -= wa1->Weapons[ bomb ].size();
            wa1->weaponCount -= wa1->Weapons[ arrow ].size();
            wa1->Weapons[ bomb ].clear();
            wa1->Weapons[ arrow ].clear();
            if ( !disableOutput )
                std::cout << std::setw( 3 ) << std::setfill( '0' ) << hour << ':' << std::setw( 2 ) << std::setfill( '0' ) << minute << " both red " << warrior[ 0 ]->name << " "
                          << warrior[ 0 ]->number << " and blue " << warrior[ 1 ]->name << " " << warrior[ 1 ]->number << " were alive in city " << this->count << std::endl;
            if ( warrior[ 0 ]->name[ 0 ] == 'd' )
                warrior[ 0 ]->yell( this );
            if ( warrior[ 1 ]->name[ 0 ] == 'd' )
                warrior[ 1 ]->yell( this );
            break;
        }
    }
    if ( warrior[ 0 ] != nullptr )
        warrior[ 0 ]->update();
    if ( warrior[ 1 ] != nullptr )
        warrior[ 1 ]->update();
}

int main() {
    int testCase;
    std::cin >> testCase;
    int currentTestCase = 0;
    while ( currentTestCase < testCase ) {
        int health;
        int T, t = 0;
        disableOutput = 0;
        std::cin >> health >> m >> k >> T;
        Red.reset( new Headquarter( 0 ) ), Blue.reset( new Headquarter( m + 1 ) );
        Red->Order[ 0 ] = 2, Red->Order[ 1 ] = 3, Red->Order[ 2 ] = 4, Red->Order[ 3 ] = 1, Red->Order[ 4 ] = 0;
        Blue->Order[ 0 ] = 3, Blue->Order[ 1 ] = 0, Blue->Order[ 2 ] = 1, Blue->Order[ 3 ] = 2, Blue->Order[ 4 ] = 4;
        Red->color = red, Blue->color = blue;
        currentTestCase++;
        hour = 0;
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
            if ( t > T )
                break;
            Red->Summon();
            Blue->Summon();
            minute = 5, t += 5;
            if ( t > T )
                break;
            for ( int i = 1; i <= m; i++ )
                Citys[ i ]->runaway();
            minute = 10, t += 5;
            for ( int i = 0; i <= m + 1; i++ )
                Citys[ i ]->walk();
            for ( int i = 0; i <= m + 1; i++ )
                Citys[ i ]->update();
            Red->update(), Blue->update();
            if ( Red->died() || Blue->died() )
                break;
            t += 25;
            minute = 35;
            if ( t > T )
                break;
            for ( int i = 1; i <= m; i++ )
                Citys[ i ]->rub();
            t += 5;
            minute = 40;
            if ( t > T )
                break;
            for ( int i = 1; i <= m; i++ )
                Citys[ i ]->fight();
            t += 10;
            minute = 50;
            if ( t > T )
                break;
            Red->report(), Blue->report();
            t += 5;
            minute = 55;
            if ( t > T )
                break;
            for ( int i = 1; i <= m; i++ )
                Citys[ i ]->report();
            hour++;
            minute = 0;
            t += 5;
        }
    }
}

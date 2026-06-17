#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>
#include "Laba2/Sequence.h"
#include "LazySequence.h"

class EndOfStream : public std::runtime_error {
public:
    explicit EndOfStream( const std::string& message ) : std::runtime_error( message ) { }
};

template <typename T>
class ReadOnlyStream {
public:
    typedef T ( *DeserializerFunc )( const std::string& );

    explicit ReadOnlyStream( const Sequence<T>* sequence )
            : source_sequence_( sequence ), current_position_( 0 ), is_open_( false ) { }

    explicit ReadOnlyStream( const ReadOnlyStream<T>& other )
            : source_sequence_( other.source_sequence_ ),
              current_position_( other.current_position_ ),
              is_open_( other.is_open_ ) { }

    void Open() {
        if ( !source_sequence_ )
            throw std::logic_error( "ReadOnlyStream: no data source attached" );
        is_open_ = true;
        current_position_ = 0; // Сбрасываем при открытии
    }

    void Close() {
        is_open_ = false;
    }

    bool IsEndOfStream() const {
        if ( !is_open_ ) throw std::logic_error( "ReadOnlyStream: stream is closed" );

        const DeferredSequence<T>* lazy_seq = dynamic_cast<const DeferredSequence<T>*>( source_sequence_ );

        if ( lazy_seq != nullptr && lazy_seq->GetOrdinalLength().has_infinity() ) {
            return false;
        }

        return current_position_ >= static_cast<size_t>( source_sequence_->GetLength() );
    }

    T Read() {
        if ( !is_open_ ) throw std::logic_error( "ReadOnlyStream: stream is closed" );
        if ( IsEndOfStream() ) throw EndOfStream( "ReadOnlyStream: reached the end of the stream" );

        T item = source_sequence_->Get( current_position_ );
        current_position_++;
        return item;
    }

    std::size_t GetPosition() const {
        return current_position_;
    }

    bool CanSeek() const { return true; }

    std::size_t Seek( std::size_t index ) {
        if ( !is_open_ ) throw std::logic_error( "ReadOnlyStream: stream is closed" );
        if ( !CanSeek() ) throw std::logic_error( "ReadOnlyStream: stream doesn't support seek" );

        if ( index > static_cast<size_t>( source_sequence_->GetLength() ) )
            throw std::out_of_range( "ReadOnlyStream: seek index out of bounds" );

        current_position_ = index;
        return current_position_;
    }

    bool CanGoBack() const { return true; }

private:
    const Sequence<T>* source_sequence_;
    std::size_t        current_position_;
    bool               is_open_;
};

template <typename T>
class WriteOnlyStream {
public:
    typedef std::string ( *SerializerFunc )( const T& );

    explicit WriteOnlyStream( Sequence<T>* sequence ) : destination_sequence_( sequence ), current_position_( 0 ), is_open_( false ) {
        if ( destination_sequence_ )
            current_position_ = static_cast<size_t>( destination_sequence_->GetLength() );
    }

    void Open() {
        if ( !destination_sequence_ )
            throw std::logic_error( "WriteOnlyStream: no data destination attached" );
        is_open_ = true;
    }

    void Close() {
        is_open_ = false;
    }

    std::size_t GetPosition() const {
        return current_position_;
    }

    std::size_t Write( const T& item ) {
        if ( !is_open_ ) throw std::logic_error( "WriteOnlyStream: stream is closed" );

        Sequence<T>* new_seq = destination_sequence_->Append( item );
        if ( new_seq != destination_sequence_ ) {
            destination_sequence_ = new_seq;
        }

        current_position_++;
        return current_position_;
    }

private:
    Sequence<T>* destination_sequence_;
    std::size_t  current_position_;
    bool         is_open_;
};
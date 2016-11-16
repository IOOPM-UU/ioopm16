package lecture21;

import java.util.Iterator;

/// No need to import -- we can use full name of class directly here
public class List<T> implements java.lang.Iterable<T> {
    private class Link<T> {
        Link<T> next;
        T element;

        public Link(T element) {
            this.element = element;
        }
    }

    public static class BadIndexException extends Exception {
        public BadIndexException(String msg) {
            super(msg);
        }
    }

    /// Dummy object always first in list
    private final Link<T> first = new Link<T>(null);
    private Link<T> last = first;

    public List(T element) {
        this.prepend(element);
    }

    public void prepend(T element) {
        this.last = new Link<T>(element);
        this.first.next = this.last;
    }

    public void append(T element) {
        this.last.next = new Link<T>(element);
        this.last = this.last.next;
    }

    public T get(int index) throws BadIndexException {
        Iterator<T> iter = this.iterator();

        for (int i = 1; iter.hasNext() && i < index; ++i) iter.next();

        if (iter.hasNext()) {
            return iter.next();
        } else {
            throw new List.BadIndexException("Index " + index + " out of bounds!");
        }
    }

    /// Returns an iterator object for this list
    public Iterator<T> iterator() {
        return new ListIterator<T>(this.first);
    }

    /// Overriding Object.equals()
    public boolean equals(Object object) {
        if (object instanceof List) {
            /// This type cast is always unsound in Java because Java represents T as Object at run-time
            /// However, this matters little -- because we only want to compare as objects! 
            return this.equals((List<T>) object);

            /// This is a fix, but outside of the scope of what we have seen thus far:
            /// return this.equals((List<?>) object);
        } else {
            return false;
        }
    }

    /// Overloading List.equals()
    public boolean equals(List<T> otherList) {
        Iterator<T> a = this.iterator();
        Iterator<T> b = otherList.iterator();

        while (a.hasNext() && b.hasNext()) {
            T e1 = a.next();
            T e2 = b.next();

            if (e1.equals(e2) == false) return false;
        }

        return a.hasNext() == b.hasNext(); // Both should end at the same time
    }

    // Bonus material! 
    // If we use the List<?> type in equals(Object), we should write like this
    // public boolean equals(List<?> otherList) {
    //     Iterator<?> a = this.iterator();
    //     Iterator<?> b = otherList.iterator();

    //     while (a.hasNext() && b.hasNext()) {
    //         Object e1 = a.next();
    //         Object e2 = b.next();

    //         if (e1.equals(e2) == false) return false;
    //     }

    //     return a.hasNext() == b.hasNext(); // Both should end at the same time
    // }

    private class ListIterator<T> implements java.util.Iterator<T> {
        private Link<T> current;

        ListIterator(Link<T> link) {
            this.current = link;
        }

        public boolean hasNext() {
            return current.next != null;
        }

        public T next() {
            current = current.next;
            return current.element;
        }

        public void remove() {
            throw new NotimplementedException("remove()");
        }
    }
}

class NotimplementedException extends RuntimeException {
    public NotimplementedException(String msg) {
        super(msg + " is not implemented");
    }
}

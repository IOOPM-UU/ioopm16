// A very simple linked list, with a "dummy link" at the head of
// the list always. (The reason for this node is to simplify the
// logic -- e.g., there always exists a first node, etc.)
// I will be extending it later to demonstrate other parts of Java
// and OOP (object-oriented programming)

/// Class is public, visible to all
public class List {
    /// Create a first link (the dummy) -- final field cannot be reassigned
    private final Link first = new Link();
    /// "this" denotes the current list object
    private Link last  = this.first;

    /// Constructor for the empty list (not necessary)
    public List() {}

    public void append(Object element) {
        /// Create a new last link, using the Link constructor taking 1 argument
        Link newLast = new Link(element);
        this.last.setNext(newLast);
        this.last = newLast;
    }

    /// Java has a proper boolean type, integers are not truth values
    public boolean isEmpty() {
        return this.first == this.last;
    }

    public void prepend(Object element) {
        /// Use the Link constructor taking 2 arguments
        Link newLink = new Link(element, this.first.getNext());
        if (this.isEmpty()) {
            this.last = newLink;
        }
        this.first.setNext(newLink);
    }

    /// The real work is delegated to findPrevious, which can be
    /// implemented, because we have the dummy node -- so there is
    /// always a previous node! 
    public void insert(Object element, int idx) {
        Link prev = this.first.findPrevious(idx);

        if (prev != null) {
            prev.setNext(new Link(element, prev.getNext()));
        }
    }

    /// The real work is delegated to findPrevious, which can be
    /// implemented, because we have the dummy node -- so there is
    /// always a previous node! 
    public Object get(int idx) {
        Link prev = this.first.findPrevious(idx);
        return prev == null ? null : prev.getElement();
    }

    /// The real work is delegated to findPrevious, which can be
    /// implemented, because we have the dummy node -- so there is
    /// always a previous node! 
    public Object remove(int idx) {
        Link prev = this.first.findPrevious(idx);

        if (prev != null) {
            /// zapNext() removes the next node, so if a-->b-->c
            /// and a.zapNext(), then a-->c, and b's element is
            /// returned
            return prev.zapNext();
        }

        return null; 
    }

    /// The link class is private and nested, this essentially
    /// means that each list _object_ has its own link _class_,
    /// which it uses to create its links. The class is not
    /// visible outside of the List class.
    private class Link {
        /// The attributes (aka instance variables or fields) in
        /// Link are private -- thus accesses go via set/get
        /// methods
        private Link next;
        /// Another use of final! 
        private final Object element;

        /// Link() ==> Link(null, null)
        public Link() {
            this(null, null);
        }
        
        /// Link(o) ==> Link(o, null)
        public Link(Object element) {
            this(element, null);
        }
        
        /// Constructs a link with a given element and next link
        public Link(Object element, Link next) {
            this.element = element;
            this.next = next;
        }

        public Link getNext() { return this.next; }
        public boolean hasNext() { return this.next != null; }
        public void setNext(Link next) {
            this.next = next;
        }
        
        public Link getElement() { return this.next; }

        /// Removes the next link, so if a-->b-->c and a.zapNext()
        /// to a-->c, and b's element is returned.
        public Object zapNext() {
            if (this.next == null) return null;

            Object element = this.next.element;
            this.next = this.next.next;
            return element;
        }

        /// Return the node previous to the one sought, so if
        /// dummy-->a-->b-->c, dummy.findPrevious(0) returns dummy,
        /// dummy.findPrevious(2) returns b, etc.
        /// Note the recursive implementation -- which calls the
        /// findPrevious() on the next link.
        public Link findPrevious(int idx) {
            if (idx == 0) {
                return this;
            } else {
                return this.next == null ? null : this.findPrevious(idx - 1);
            }
        }

        /// Return the string representation of the element
        public String toString() {
            return this.element.toString();
        }
    }

    /// Return a string representation of the list "[a, b, c]"
    /// where a, b, c are string representations of the 3 elements
    public String toString() {
        StringBuilder result = new StringBuilder();
        
        Link cursor = this.first.getNext();

        result.append("[");
        for (; cursor != null; cursor = cursor.getNext()) {
            result.append(cursor.toString());

            if (cursor.hasNext()) {
                result.append(", ");
            }
        }
        result.append("]");

        return result.toString();
    }

    /// Here is where the program starts! Note that it is
    /// contained inside the List class. To compile, write `javac
    /// List.java`. To run, write `java List`. The latter will
    /// call the main method on the list _class_.
    ///
    /// The keyword static means main is part of the list class,
    /// not part of objects created from the list class.
    public static final void main(String[] args) {
        List l = new List();

        /// Note how we can add elements of different types into
        /// the list
        l.append("Tjugosex");
        l.append(4711);

        /// Since each element is asked to produce its own string
        /// representation, this works just fine!
        System.out.println(l);
    }
}

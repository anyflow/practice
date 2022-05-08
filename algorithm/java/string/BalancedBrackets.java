class BalancedBrackets {
    public static boolean isOpening(char ch) {
        return ch == '{' || ch == '[' || ch == '(';
    }
    
    public static boolean isPair(char opening, char closing) {
        switch(opening) {
            case '(': return closing == ')';
            case '{': return closing == '}';
            case '[': return closing == ']';
            default: return false;
        }
    }
    public static boolean isBalanced(String expression) {
        Stack stack = new Stack();
        
        for(int i=0; i<expression.length(); ++i) {
            char ch = expression.charAt(i);
            if(isOpening(ch)) {
                stack.push(new Node(ch));
            }
            else {
                Node node = stack.pop();
                if(node == null ||
                  !isPair(node.ch, ch)) { return false; }
            }
        }
        
        return stack.top == null;
    }
  
    public static void main(String[] args) {
      System.out.println(isBalanced("{[()]}"));
    }
    
    static class Node {
        Character ch;
        Node next;
        
        Node(char ch) {
            this.ch = ch;
        }
    }
    
    static class Stack {
        Node top;
        
        void push(Node node) {
            if(top == null) {
                top = node;
                return;
            }
            
            node.next = top;
            top = node;
        }
        
        Node pop() {
            if(top == null) {
                return null;
            }
            
            Node ret = top;
            top = top.next;
            
            return ret;
        }
    }
}
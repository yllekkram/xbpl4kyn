#ifndef MESSAGE_HPP
#define MESSAGE_HPP

class Message {
	public:
		Message(char* buffer, int len);
		~Message();

		char* getBuffer() const;
		int getLen() const;

	private:
		char* buffer;
		int len;
};

#endif

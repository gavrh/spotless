CREATE TABLE accounts (
    account_id VARCHAR(128) NOT NULL UNIQUE,
    account_2fa_secret VARCHAR(26) NOT NULL UNIQUE,
    account_setup_complete BOOL NOT NULL DEFAULT FALSE,
);
